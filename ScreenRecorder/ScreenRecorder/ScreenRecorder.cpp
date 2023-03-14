#pragma comment (lib, "user32.lib")
#include "ScreenRecorder.h"
#include <QMessageBox>
#include <QFile>
#include <QMouseEvent>
#include <QScreen>
#include <QRect>
#include <QPropertyAnimation>
#include "Helper.h"


#define TIME_OUT_MILLISECONDS 1000

// style sheet
static QString qssLumosPath = ":/Style/Asset/Style/lumos.qss";
static QString qssNoxPath = ":/Style/Asset/Style/nox.qss";
static QString startBtnStyle = "QPushButton#btnRec{border-image:url(:/Icon/Asset/Icon/start.svg)}"
"QPushButton::hover#btnRec{border-image:url(:/Icon/Asset/Icon/start_hover.svg)}";
static QString stopBtnStyle = "QPushButton#btnRec{border-image:url(:/Icon/Asset/Icon/stop.svg)}"
"QPushButton::hover#btnRec{border-image:url(:/Icon/Asset/Icon/stop_hover.svg)}";
static QString pinBtnStyle = "QPushButton#btnPin{border-image:url(:/Icon/Asset/Icon/pin.svg)}"
"QPushButton::hover#btnPin{border-image:url(:/Icon/Asset/Icon/pin_hover.svg)}";
static QString pinnedBtnStyle = "QPushButton#btnPin{border-image:url(:/Icon/Asset/Icon/pinned.svg)}"
"QPushButton::hover#btnPin{border-image:url(:/Icon/Asset/Icon/pinned_hover.svg)}";

ScreenRecorder::ScreenRecorder(QWidget *parent)
    : QWidget(parent), m_ptrUtilsWrapper(new UtilsWrapper)
{
    ui.setupUi(this);

    // widgets
    QPixmap pixLogo(":/Icon/Asset/Icon/logo.svg");
    ui.qlabelLogo->setPixmap(pixLogo);

    pMenu = new QMenu(this);
    pActLumos = new QAction("Lumos", this);
    pActNox = new QAction("Nox", this);
    pMenu->addAction(pActLumos);
    pMenu->addAction(pActNox);
    ui.btnStyle->setMenu(pMenu);

    ui.lcdNumber->setDigitCount(8); // note that the default digit number is 5, this number can be access by "int digitCount()"
    ui.lcdNumber->display("00:00:00");
    m_ptrTimer = new QTimer(this);
    ui.checkBoxOutputAudio->setChecked(true);
    ui.checkBoxInputAudio->setChecked(false);

    // get the window instance
    pWin = this->window();

    // signal-slot
    connect(m_ptrTimer, &QTimer::timeout, this, &ScreenRecorder::on_timer_timeout);
    connect(pActLumos, &QAction::triggered, [=] {LoadQSS(qssLumosPath); });
    connect(pActNox, &QAction::triggered, [=] {LoadQSS(qssNoxPath); });

    // event filter
    installEventFilter(this);

    // window
    setWindowFlags(Qt::FramelessWindowHint);

    // style sheet
    LoadQSS(qssLumosPath);

    // model initialization
    if (!m_ptrUtilsWrapper->InitUtils()) {
        QMessageBox::warning(this, "Warn", "Initialization failed!");
        exit(0);
    }
}

ScreenRecorder::~ScreenRecorder()
{}

void ScreenRecorder::on_btnSearch_clicked()
{
    // search for monitor & window sources
    m_recMonitorObj.clear();
    m_recWindowObj.clear();
    m_ptrUtilsWrapper->SearchSource(REC_MONITOR, m_recMonitorObj);
    m_ptrUtilsWrapper->SearchSource(REC_WINDOW, m_recWindowObj);

    // display sources in the combobox
    ui.comboBoxRecObj->clear();
    for (const string& obj : m_recMonitorObj) {
        ui.comboBoxRecObj->addItem(QString::fromStdString(obj), QVariant(REC_MONITOR));
    }
    for (const string& obj : m_recWindowObj) {
        ui.comboBoxRecObj->addItem(QString::fromStdString(obj), QVariant(REC_WINDOW));
    }
}

void ScreenRecorder::on_btnRec_clicked()
{

    if (!m_isRecording) {
        // set video source
        const string rec_obj = ui.comboBoxRecObj->currentText().toStdString();
        int rec_type = ui.comboBoxRecObj->currentData().toInt();
        if (rec_type == REC_MONITOR) {
            m_recMonitorObj.clear();
            m_ptrUtilsWrapper->SearchSource(REC_MONITOR, m_recMonitorObj);
            if (StringInVector(rec_obj, m_recMonitorObj)) {
                m_ptrUtilsWrapper->SetVideoSource(REC_MONITOR, rec_obj.c_str());
            }
            else {
                QMessageBox::warning(this, "Error", "The selected monitor source does not exist!");
                return;
            }
        }
        else if (rec_type == REC_WINDOW) {
            m_recWindowObj.clear();
            m_ptrUtilsWrapper->SearchSource(REC_WINDOW, m_recWindowObj);
            if (StringInVector(rec_obj, m_recWindowObj)) {
                m_ptrUtilsWrapper->SetVideoSource(REC_WINDOW, rec_obj.c_str());
            }
            else {
                QMessageBox::warning(this, "Error", "The selected window source does not exist!");
                return;
            }
        }

        // set audio source
        audio_source.useOutputAudio = ui.checkBoxOutputAudio->isChecked();
        audio_source.useInputAudio = ui.checkBoxInputAudio->isChecked();
        m_ptrUtilsWrapper->SetAudioSource(audio_source);

        // start recording
        if (m_ptrUtilsWrapper->StartRec()) {
            m_ptrTimer->start(TIME_OUT_MILLISECONDS);
            ui.btnRec->setStyleSheet(stopBtnStyle);
            m_isRecording = true;
        }
        else {
            QMessageBox::warning(this, "Error", "Can not start recording!");
        }
    }
    else {
        // stop recording
        if (m_ptrUtilsWrapper->StopRec()) {
            m_ptrTimer->stop();
            m_RecSeconds = 0;
            ui.lcdNumber->display("00:00:00");
            ui.btnRec->setStyleSheet(startBtnStyle);
            m_isRecording = false;
        }
    }
}

void ScreenRecorder::on_timer_timeout()
{
    m_RecSeconds++;

    int hour = m_RecSeconds / 3600;
    int minute = (m_RecSeconds - hour * 3600) / 60; 
    int second = m_RecSeconds % 60;

    string time = format("{:02d}:{:02d}:{:02d}", hour, minute, second);
    ui.lcdNumber->display(time.c_str());
}

void ScreenRecorder::LoadQSS(QString qssPath)
{
    QFile file(qssPath);
    QString qss;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qss = file.readAll();
    }

    file.close();

    this->setStyleSheet(qss);
}

void ScreenRecorder::mousePressEvent(QMouseEvent* event)
{
    if (ui.titleBarWidget && ui.titleBarWidget->underMouse()) {
        if (ReleaseCapture()) {
            if (pWin->isTopLevel()) {
                SendMessage(HWND(pWin->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, NULL);
            }
        }
    }
}

void ScreenRecorder::on_btnSet_clicked()
{

}

void ScreenRecorder::on_btnStyle_clicked()
{

}

void ScreenRecorder::on_btnPin_clicked()
{
    m_isPinned = !m_isPinned;
    if (m_isPinned) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        ui.btnPin->setStyleSheet(pinnedBtnStyle);
    }
    else {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        ui.btnPin->setStyleSheet(pinBtnStyle);
    }
}

void ScreenRecorder::on_btnMin_clicked()
{
    pWin->showMinimized();
}

void ScreenRecorder::on_btnTray_clicked()
{
}

void ScreenRecorder::on_btnClose_clicked()
{
    pWin->close();
}

bool ScreenRecorder::eventFilter(QObject* obj, QEvent* event)
{
    int residualPixel = 2;
    int animationDuration = 200;

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    QRect screenRect = primaryScreen->availableGeometry();
    QRect windowRect = frameGeometry();

    // check if the window is at the edge of the screen
    if (event->type() == QEvent::Move) {
        m_winPos.atTopEdge = (windowRect.top() == screenRect.top());
        m_winPos.atLeftEdge = (windowRect.left() == screenRect.left());
        m_winPos.atRightEdge = (windowRect.right() == screenRect.right());
    }

    // prevent the window move outside the screen
    if (event->type() == QEvent::MouseButtonRelease) {
        if (windowRect.left() < screenRect.left()) {
            move(screenRect.left(), windowRect.y());
        }

        if (windowRect.right() > screenRect.right()) {
            move(screenRect.right() - windowRect.right() + windowRect.left(), windowRect.y());
        }
    }

    if (m_winState.isHidedAtTop && event->type() == QEvent::Enter) {
        QRect windowRect = frameGeometry();
        QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
        posAnimation->setDuration(animationDuration);
        posAnimation->setStartValue(windowRect.topLeft());
        posAnimation->setEndValue(QPoint(windowRect.x(), 0));
        posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        m_winState.isHidedAtTop = false;
    }

    if (m_winPos.atTopEdge && event->type() == QEvent::Leave) {
        QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
        posAnimation->setDuration(animationDuration);
        posAnimation->setStartValue(windowRect.topLeft());
        posAnimation->setEndValue(QPoint(windowRect.x(), residualPixel - windowRect.bottomLeft().y()));
        posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        m_winPos.atTopEdge = false;
        m_winState.isHidedAtTop = true;
    }

    if (m_winState.isHidedAtLeft && event->type() == QEvent::Enter) {
        QRect windowRect = frameGeometry();
        QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
        posAnimation->setDuration(animationDuration);
        posAnimation->setStartValue(windowRect.topLeft());
        posAnimation->setEndValue(QPoint(0, windowRect.y()));
        posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        m_winState.isHidedAtLeft = false;
    }

    if (m_winPos.atLeftEdge && event->type() == QEvent::Leave) {
        QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
        posAnimation->setDuration(animationDuration);
        posAnimation->setStartValue(windowRect.topLeft());
        posAnimation->setEndValue(QPoint(residualPixel - windowRect.right(), windowRect.y()));
        posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        m_winPos.atLeftEdge = false;
        m_winState.isHidedAtLeft = true;
    }

    if (m_winState.isHidedAtRight && event->type() == QEvent::Enter) {
        QRect windowRect = frameGeometry();
        QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
        posAnimation->setDuration(animationDuration);
        posAnimation->setStartValue(windowRect.topLeft());
        posAnimation->setEndValue(QPoint(screenRect.right()-windowRect.right()+windowRect.left(), windowRect.y()));
        posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        m_winState.isHidedAtRight = false;
    }

    if (m_winPos.atRightEdge && event->type() == QEvent::Leave) {
        QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
        posAnimation->setDuration(animationDuration);
        posAnimation->setStartValue(windowRect.topLeft());
        posAnimation->setEndValue(QPoint(screenRect.right()-residualPixel, windowRect.y()));
        posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        m_winPos.atRightEdge = false;
        m_winState.isHidedAtRight = true;
    }

    return QWidget::eventFilter(obj, event);
}

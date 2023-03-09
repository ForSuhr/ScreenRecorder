#include "ScreenRecorder.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFile>

#define TIME_OUT_MILLISECONDS 1000

// style sheet
static QString qssNoxPath = ":/Style/Asset/Style/nox.qss";
static QString qssLumosPath = ":/Style/Asset/Style/lumos.qss";
static QString playBtnStyle = "QPushButton{border-radius:48px;border-image:url(:/Icon/Asset/Icon/play.svg)}"
"QPushButton::hover{border-radius:48px;border-image:url(:/Icon/Asset/Icon/play_hover.svg)}";
static QString stopBtnStyle = "QPushButton{border-radius:48px;border-image:url(:/Icon/Asset/Icon/stop.svg)}"
"QPushButton::hover{border-radius:48px;border-image:url(:/Icon/Asset/Icon/stop_hover.svg)}";

ScreenRecorder::ScreenRecorder(QWidget *parent)
    : QWidget(parent), m_pUtilsWrapper(new UtilsWrapper)
{
    ui.setupUi(this);

    // style sheet
    LoadQSS(qssNoxPath);

    // title bar
    m_pTitle = new TitleBar(this);
    QHBoxLayout* hBox = new QHBoxLayout(this);
    m_pTitle->setFixedWidth(this->width());
    hBox->addWidget(m_pTitle, 0, Qt::AlignTop);

    // widgets
    ui.lcdNumber->setDigitCount(8); // note that the default digit number is 5, this number can be access by "int digitCount()"
    ui.lcdNumber->display("00:00:00");
    m_pTimer = new QTimer(this);

    // signal-slot
    connect(m_pTimer, &QTimer::timeout, this, &ScreenRecorder::on_timer);

    // window
    setWindowFlags(Qt::FramelessWindowHint);

    // model initialization
    if (!m_pUtilsWrapper->InitUtils()) {
        QMessageBox::warning(this, "Warn", "Initialization failed!");
        exit(0);
    }
}

ScreenRecorder::~ScreenRecorder()
{}

void ScreenRecorder::LoadQSS(QString qssPath)
{
    QFile file(qssPath);
    QString qss;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString qss = file.readAll();
    }

    file.close();

    this->setStyleSheet(qss);
}

void ScreenRecorder::on_btnRec_clicked()
{
    if (!m_isRecording) {
        if (m_pUtilsWrapper->StartRec()) {
            m_pTimer->start(TIME_OUT_MILLISECONDS);
            ui.btnRec->setStyleSheet(stopBtnStyle);
            m_isRecording = true;
        }
        else {
            QMessageBox::warning(this, "Error", "Can not start recording!");
        }
    }
    else {
        if (m_pUtilsWrapper->StopRec()) {
            m_pTimer->stop();
            m_RecSeconds = 0;
            ui.lcdNumber->display("00:00:00");
            ui.btnRec->setStyleSheet(playBtnStyle);
            m_isRecording = false;
        }
    }
}

void ScreenRecorder::on_timer()
{
    m_RecSeconds++;

    int hour = m_RecSeconds / 3600;
    int minute = (m_RecSeconds - hour * 3600) / 60; 
    int second = m_RecSeconds % 60;

    string time = std::format("{:02d}:{:02d}:{:02d}", hour, minute, second);
    ui.lcdNumber->display(time.c_str());
}

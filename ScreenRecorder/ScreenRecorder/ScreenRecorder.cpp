#include "ScreenRecorder.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFile>
#include "Helper.h"


#define TIME_OUT_MILLISECONDS 1000

// style sheet
static QString qssLumosPath = ":/Style/Asset/Style/lumos.qss";
static QString qssNoxPath = ":/Style/Asset/Style/nox.qss";
static QString playBtnStyle = "QPushButton#btnRec{border-image:url(:/Icon/Asset/Icon/play.svg)};"
"QPushButton::hover#btnRec{border-image:url(:/Icon/Asset/Icon/play_hover.svg)};";
static QString stopBtnStyle = "QPushButton#btnRec{border-image:url(:/Icon/Asset/Icon/stop.svg)};"
"QPushButton::hover#btnRec{border-image:url(:/Icon/Asset/Icon/stop_hover.svg)};";

ScreenRecorder::ScreenRecorder(QWidget *parent)
    : QWidget(parent), m_ptrUtilsWrapper(new UtilsWrapper)
{
    ui.setupUi(this);

    // title bar
    m_pTitle = new TitleBar(this);
    QHBoxLayout* hBox = new QHBoxLayout(this);
    m_pTitle->setFixedWidth(this->width());
    hBox->addWidget(m_pTitle, 0, Qt::AlignTop);

    // widgets
    ui.lcdNumber->setDigitCount(8); // note that the default digit number is 5, this number can be access by "int digitCount()"
    ui.lcdNumber->display("00:00:00");
    m_ptrTimer = new QTimer(this);
    ui.checkBoxOutputAudio->setChecked(true);
    ui.checkBoxInputAudio->setChecked(false);

    // signal-slot
    connect(m_ptrTimer, &QTimer::timeout, this, &ScreenRecorder::on_timer_timeout);
    connect(m_pTitle->pActLumos, &QAction::triggered, [=] {LoadQSS(qssLumosPath); });
    connect(m_pTitle->pActNox, &QAction::triggered, [=] {LoadQSS(qssNoxPath); });

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
            ui.btnRec->setStyleSheet(playBtnStyle);
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

    string time = std::format("{:02d}:{:02d}:{:02d}", hour, minute, second);
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

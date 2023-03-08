#include "ScreenRecorder.h"
#include <QMessageBox>

#define TIME_OUT_MILLISECONDS 1000

ScreenRecorder::ScreenRecorder(QWidget *parent)
    : QWidget(parent), m_pUtilsWrapper(new UtilsWrapper)
{
    ui.setupUi(this);

    ui.lcdNumber->setDigitCount(8); // note that the default digit number is 5, this number can be access by "int digitCount()"
    ui.lcdNumber->display("00:00:00");
    m_pTimer = new QTimer(this);

    connect(m_pTimer, &QTimer::timeout, this, &ScreenRecorder::on_timer);

    if (!m_pUtilsWrapper->InitUtils()) {
        QMessageBox::warning(this, "Warn", "Initialization failed!");
        exit(0);
    }

}

ScreenRecorder::~ScreenRecorder()
{}

void ScreenRecorder::on_btnRec_clicked()
{
    if (!m_isRecording) {
        ui.btnRec->setText("Stop");
        if (m_pUtilsWrapper->StartRec()) {
            m_pTimer->start(TIME_OUT_MILLISECONDS);
            m_isRecording = true;
        }
        else {
            QMessageBox::warning(this, "Error", "Can not start recording!");
        }
    }
    else {
        ui.btnRec->setText("Start");
        if (m_pUtilsWrapper->StopRec()) {
            m_pTimer->stop();
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

#include "ScreenRecorder.h"
#include <QMessageBox>

ScreenRecorder::ScreenRecorder(QWidget *parent)
    : QWidget(parent), ptr_utilsWrapper(new UtilsWrapper)
{
    ui.setupUi(this);

    if (!ptr_utilsWrapper->InitUtils()) {
        QMessageBox::information(this, "Warn", "Initialization failed!");
        exit(0);
    }
}

ScreenRecorder::~ScreenRecorder()
{}


void ScreenRecorder::on_btnRec_clicked()
{
    if (!m_isRecording) {
        ui.btnRec->setText("Stop");
        m_isRecording = true;
    }
    else {
        ui.btnRec->setText("Start");
        m_isRecording = false;
    }

}
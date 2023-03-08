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
        if (ptr_utilsWrapper->StartRec()) {
            m_isRecording = true;
        }
    }
    else {
        ui.btnRec->setText("Start");
        if (ptr_utilsWrapper->StopRec()) {
            m_isRecording = false;
        }
    }
}
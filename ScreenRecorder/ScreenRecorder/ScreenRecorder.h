#pragma once

#include <QtWidgets/QWidget>
#include "ui_ScreenRecorder.h"
#include "UtilsWrapper.h"
#include <memory>

class ScreenRecorder : public QWidget
{
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = nullptr);
    ~ScreenRecorder();

public slots:
    void on_btnRec_clicked();

private:
    Ui::ScreenRecorderClass ui;

    bool m_isRecording = false;

    std::unique_ptr<UtilsWrapper> ptr_utilsWrapper;
};

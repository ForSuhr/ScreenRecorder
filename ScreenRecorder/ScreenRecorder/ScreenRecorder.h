#pragma once

#include <QtWidgets/QWidget>
#include "ui_ScreenRecorder.h"
#include "UtilsWrapper.h"
#include <memory>
#include <QTimer>

class ScreenRecorder : public QWidget
{
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = nullptr);
    ~ScreenRecorder();

public slots:
    void on_btnRec_clicked();
    void on_timer();

private:
    Ui::ScreenRecorderClass ui;
    std::unique_ptr<UtilsWrapper> m_pUtilsWrapper;
    QTimer* m_pTimer;

    bool m_isRecording = false;
    int m_RecSeconds = 0;
};

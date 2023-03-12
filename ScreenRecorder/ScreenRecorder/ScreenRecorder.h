#pragma once
#include <QtWidgets/QWidget>
#include "ui_ScreenRecorder.h"
#include "TitleBar.h"
#include "UtilsWrapper.h"
#include <memory>
#include <QTimer>


class ScreenRecorder : public QWidget
{
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = nullptr);
    ~ScreenRecorder();

    void LoadQSS(QString qssPath);

public slots:
    void on_btnSearch_clicked();
    void on_btnRec_clicked();
    void on_timer_timeout();

private:
    Ui::ScreenRecorderClass ui;
    TitleBar* m_pTitle;
    std::unique_ptr<UtilsWrapper> m_ptrUtilsWrapper;
    QTimer* m_ptrTimer;
    REC_OBJ m_recMonitorObj;
    REC_OBJ m_recWindowObj;
    AUDIO_SOURCE audio_source;

    bool m_isRecording = false;
    int m_RecSeconds = 0;
};

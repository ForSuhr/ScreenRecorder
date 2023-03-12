#pragma once
#include <QtWidgets/QWidget>
#include <qt_windows.h>
#include "ui_ScreenRecorder.h"
#include "UtilsWrapper.h"
#include <memory>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>


class ScreenRecorder : public QWidget
{
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = Q_NULLPTR);
    ~ScreenRecorder();
    void LoadQSS(QString qssPath);

    QAction* pActNox = nullptr;
    QAction* pActLumos = nullptr;

public slots:
    void on_btnSearch_clicked();
    void on_btnRec_clicked();
    void on_timer_timeout();
    void on_btnSet_clicked();
    void on_btnStyle_clicked();
    void on_btnMin_clicked();
    void on_btnClose_clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    Ui::ScreenRecorderClass ui;
    std::unique_ptr<UtilsWrapper> m_ptrUtilsWrapper;
    QTimer* m_ptrTimer;
    QSystemTrayIcon* m_systemTray;
    QWidget* pWin = nullptr;
    QMenu* pMenu = nullptr;
    REC_OBJ m_recMonitorObj;
    REC_OBJ m_recWindowObj;
    AUDIO_SOURCE audio_source;

    bool m_isRecording = false;
    int m_RecSeconds = 0;
};

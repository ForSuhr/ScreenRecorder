#pragma once
#include <QtWidgets/QWidget>
#include <qt_windows.h>
#include "ui_ScreenRecorder.h"
#include "StyleSheet.h"
#include "ConfigPanel.h"
#include "ConfigParser.h"
#include "UtilsWrapper.h"
#include <memory>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>


struct WINDOW_POS {
    bool atTopEdge = false;
    bool atLeftEdge = false;
    bool atRightEdge = false;
};

struct WINDOW_STATE {
    bool isHidedAtTop = false;
    bool isHidedAtLeft = false;
    bool isHidedAtRight = false;
};


class ScreenRecorder : public QWidget
{
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = Q_NULLPTR);
    ~ScreenRecorder();

public slots:
    // title bar
    void on_btnConfig_clicked();
    void on_btnStyle_clicked();
    void on_btnPin_clicked();
    void on_btnMin_clicked();
    void on_btnTray_clicked();
    void on_btnClose_clicked();
    // main window
    void on_btnSearch_clicked();
    void on_btnRec_clicked();
    void on_timer_timeout();

protected:
    void mousePressEvent(QMouseEvent* event);
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // field for model
    std::unique_ptr<UtilsWrapper> m_ptrUtilsWrapper;
    bool m_isRecording = false;
    int m_RecSeconds = 0;
    bool m_isPinned = false;
    REC_OBJ m_recMonitorObj;
    REC_OBJ m_recWindowObj;
    AUDIO_SOURCE audio_source;
    // field for view
    Ui::ScreenRecorderClass ui;
    QSystemTrayIcon* m_systemTray;
    ConfigPanel* m_configPanel;
    // field for controller
    QTimer* m_ptrTimer;
    QWidget* pWin = nullptr;
    WINDOW_POS m_winPos;
    WINDOW_STATE m_winState;
};

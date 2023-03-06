#pragma once

#include <QtWidgets/QWidget>
#include "ui_ScreenRecorder.h"

class ScreenRecorder : public QWidget
{
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = nullptr);
    ~ScreenRecorder();

private:
    Ui::ScreenRecorderClass ui;
};

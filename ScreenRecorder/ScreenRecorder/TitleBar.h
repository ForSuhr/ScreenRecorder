#pragma once

#include <QWidget>
#include "ui_TitleBar.h"

class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent = nullptr);
	~TitleBar();

protected:
	void mousePressEvent(QMouseEvent* event);

private slots:
	void on_btnSet_clicked();
	void on_btnSkin_clicked();
	void on_btnMin_clicked();
	void on_btnClose_clicked();

private:
	Ui::TitleBarClass ui;

	QWidget* pWin;
};

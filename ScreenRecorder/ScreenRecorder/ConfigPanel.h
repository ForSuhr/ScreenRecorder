#pragma once

#include <QWidget>
#include "ui_ConfigPanel.h"

class ConfigPanel : public QWidget
{
	Q_OBJECT

public:
	ConfigPanel(QWidget *parent = nullptr);
	~ConfigPanel();

public slots:
	// config item list
	void on_btnGeneral_clicked();
	void on_btnAV_clicked();
	// config item context
	void on_btnSelectPath_clicked();

private:
	Ui::ConfigPanelClass ui;
};

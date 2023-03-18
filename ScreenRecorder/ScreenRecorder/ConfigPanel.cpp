#include "ConfigPanel.h"
#include "ConfigParser.h"
#include <QFileDialog>

ConfigPanel::ConfigPanel(QWidget *parent)
	: QWidget(parent)
{
	// window
	setWindowFlags(Qt::WindowStaysOnTopHint);

	// config file
	ConfigParser::getInstance().checkConfig();

	// config panel
	ui.setupUi(this);
	ui.stackedWidgetConfigItemContext->setCurrentWidget(ui.pageGeneral);

	// general page
	QString currentOutputPath = QString::fromStdString(ConfigParser::getInstance().getConfig("General/OutputPath"));
	ui.lineEditOutputPath->setText(currentOutputPath);
	connect(ui.lineEditOutputPath, &QLineEdit::textChanged, [=] {
		ConfigParser::getInstance().setConfig("General/OutputPath", ui.lineEditOutputPath->text().toStdString());
	});

	// audio & video page

}

ConfigPanel::~ConfigPanel()
{}

void ConfigPanel::on_btnGeneral_clicked()
{
	ui.stackedWidgetConfigItemContext->setCurrentWidget(ui.pageGeneral);

}

void ConfigPanel::on_btnAV_clicked()
{
	ui.stackedWidgetConfigItemContext->setCurrentWidget(ui.pageAV);

}

void ConfigPanel::on_btnSelectPath_clicked()
{
	QString currentOutputPath = QString::fromStdString(ConfigParser::getInstance().getConfig("General/OutputPath"));
	QString dir = QFileDialog::getExistingDirectory(this, "Select Directory", currentOutputPath, QFileDialog::ShowDirsOnly);
	if (!dir.isEmpty())	ui.lineEditOutputPath->setText(dir);
}

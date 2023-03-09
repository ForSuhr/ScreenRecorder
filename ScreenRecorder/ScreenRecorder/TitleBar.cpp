#pragma comment (lib, "user32.lib")
#include <qt_windows.h>
#include "TitleBar.h"
#include <QMouseEvent>

static QString titleBarStyle = "background-color:rgb(255, 255, 255, 100);";

TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setStyleSheet(titleBarStyle);

	QPixmap pixLogo(":/Icon/Asset/Icon/logo.svg");
	ui.qlabelLogo->setPixmap(pixLogo);

	pWin = this->window();
}

TitleBar::~TitleBar()
{}

void TitleBar::mousePressEvent(QMouseEvent* event)
{
	if (ReleaseCapture()) {
		if (pWin->isTopLevel()) {
			SendMessage(HWND(pWin->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, NULL);
		}
	}

	event->ignore();
}

void TitleBar::on_btnSet_clicked()
{

}

void TitleBar::on_btnSkin_clicked()
{
}

void TitleBar::on_btnMin_clicked()
{
	pWin->showMinimized();
}

void TitleBar::on_btnClose_clicked()
{
	pWin->close();
}

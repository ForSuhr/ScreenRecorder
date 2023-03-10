#pragma comment (lib, "user32.lib")
#include <qt_windows.h>
#include "TitleBar.h"
#include <QMouseEvent>



TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QPixmap pixLogo(":/Icon/Asset/Icon/logo.svg");
	ui.qlabelLogo->setPixmap(pixLogo);

	pMenu = new QMenu(this);
	pActLumos = new QAction("Lumos", this);
	pActNox = new QAction("Nox", this);
	pMenu->addAction(pActLumos);
	pMenu->addAction(pActNox);
	ui.btnStyle->setMenu(pMenu);

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

void TitleBar::on_btnStyle_clicked()
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


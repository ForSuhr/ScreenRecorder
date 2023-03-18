#pragma once
#include <QWidget>
#include <QString>

// style sheet
static QString qssLumosPath = ":/Style/Asset/Style/lumos.qss";
static QString qssNoxPath = ":/Style/Asset/Style/nox.qss";
static QString startBtnStyle = "QPushButton#btnRec{border-image:url(:/Icon/Asset/Icon/start.svg)}"
"QPushButton::hover#btnRec{border-image:url(:/Icon/Asset/Icon/start_hover.svg)}";
static QString stopBtnStyle = "QPushButton#btnRec{border-image:url(:/Icon/Asset/Icon/stop.svg)}"
"QPushButton::hover#btnRec{border-image:url(:/Icon/Asset/Icon/stop_hover.svg)}";
static QString pinBtnStyle = "QPushButton#btnPin{border-image:url(:/Icon/Asset/Icon/pin.svg)}"
"QPushButton::hover#btnPin{border-image:url(:/Icon/Asset/Icon/pin_hover.svg)}";
static QString pinnedBtnStyle = "QPushButton#btnPin{border-image:url(:/Icon/Asset/Icon/pinned.svg)}"
"QPushButton::hover#btnPin{border-image:url(:/Icon/Asset/Icon/pinned_hover.svg)}";


class StyleSheet
{
public:
	static StyleSheet& getInstance();
	void loadQSS(QWidget* widget, QString qssPath);

private: 
	StyleSheet();
	~StyleSheet();
};


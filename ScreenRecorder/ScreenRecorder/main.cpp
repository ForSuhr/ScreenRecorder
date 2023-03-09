#include "ScreenRecorder.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenRecorder w;
    w.show();
    return a.exec();
}

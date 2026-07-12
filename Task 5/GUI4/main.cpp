#include <QApplication>
#include <QLoggingCategory>
#include "gamewindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameWindow w;
    w.showFullScreen();

    return a.exec();
}

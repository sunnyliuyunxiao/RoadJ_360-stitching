#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::WindowFullscreenButtonHint);
    w.show();

    return a.exec();
}

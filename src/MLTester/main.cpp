#include "MLtester.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <Windows.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Show startup image
    QPixmap startup(":/image/images/routine/startup.png");
    QSplashScreen splash(startup);
    splash.show();

    MLtester w;
    splash.finish(&w);
    w.setWindowState(Qt::WindowMaximized);
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);
    w.setFixedSize(width, height);
    w.setWindowTitle("MLColorimeter");
    w.show();

    
    return a.exec();
}

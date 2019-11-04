#include <QApplication>
#include <QSplashScreen>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    QApplication::processEvents();

    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    QApplication::addLibraryPath("/home/safran/ADLINK/Vortex_v2/Device/VortexOpenSplice/6.10.0p2/HDE/x86_64.linux/lib/");

    MainWindow mainWindow;
    mainWindow.setWindowState(Qt::WindowMaximized);
    mainWindow.show();

    splash.finish(&mainWindow);

    return QApplication::exec();
}

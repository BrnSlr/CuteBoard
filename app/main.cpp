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

    QFont ft = app.font();
    ft.setPointSizeF(8.5);
    app.setFont(ft);

    MainWindow mainWindow;
    mainWindow.setWindowState(Qt::WindowMaximized);
    mainWindow.show();

    splash.finish(&mainWindow);

    return QApplication::exec();
}

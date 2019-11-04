#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dashboard/dashboard.h"
#include "project/project.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool maximized() const;
    void setMaximized(bool maximized);

public slots:
    void showHomePage();
    void showLivePage();
    void showDesignPage();
    void showReplayPage();
    void showSettingsPage();
    void fullScreen(bool fullscreen);
    void projectSelected();

protected:
    void init();

private:
    Ui::MainWindow *ui;

    QRect mNormalGeometry;
    QAction *mHomeAction;
    QAction *mDesignAction;
    QAction *mLiveAction;
    QAction *mReplayAction;
    QAction *mSettingsAction;
};

#endif // MAINWINDOW_H

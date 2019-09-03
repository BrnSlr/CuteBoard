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
    void showSettingsPage();
    void fullScreen(bool fullscreen);
    void projectSelected();

protected:
    void init();

private:
    Ui::MainWindow *ui;

    bool mMaximized;
    QAction *mHomeAction;
    QAction *mDesignAction;
    QAction *mLiveAction;
    QAction *mSettingsAction;
};

#endif // MAINWINDOW_H

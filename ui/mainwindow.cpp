#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/settings_dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->homeWidget, SIGNAL(showLivePage()), this, SLOT(showLivePage()));
    connect(ui->homeWidget, SIGNAL(showDesignPage()), this, SLOT(showDesignPage()));
    connect(ui->dashboardWidget, SIGNAL(fullScreenMode(bool)), this, SLOT(fullScreen(bool)));

    QPixmap home_enabled_icon( ":/icons8_home_enabled_32px.png" );
    QPixmap home_disabled_icon( ":/icons8_home_disabled_32px.png" );
    QIcon home_icon( home_enabled_icon );
    home_icon.addPixmap( home_disabled_icon, QIcon::Disabled );
    mHomeAction = ui->toolBar->addAction(home_icon, "Home");
    mHomeAction->setCheckable(true);
    connect(mHomeAction, &QAction::triggered, this, &MainWindow::showHomePage);

    QPixmap live_enabled_icon( ":/icons8_circled_play_enabled_32px.png" );
    QPixmap live_disabled_icon( ":/icons8_circled_play_disabled_32px.png" );
    QIcon live_icon( live_enabled_icon );
    live_icon.addPixmap( live_disabled_icon, QIcon::Disabled );
    mLiveAction = ui->toolBar->addAction(live_icon, "Live");
    mLiveAction->setCheckable(true);
    mLiveAction->setEnabled(false);
    connect(mLiveAction, &QAction::triggered, this, &MainWindow::showLivePage);

    QPixmap playback_enabled_icon( ":/icons8_past_32px.png" );
    QPixmap playback_disabled_icon( ":/icons8_past_filled_32px.png" );
    QIcon playback_icon( playback_enabled_icon );
    playback_icon.addPixmap( playback_disabled_icon, QIcon::Disabled );
    mReplayAction = ui->toolBar->addAction(playback_icon, "Playback");
    mReplayAction->setCheckable(true);
    mReplayAction->setEnabled(false);
    connect(mReplayAction, &QAction::triggered, this, &MainWindow::showReplayPage);


    QPixmap design_enabled_icon( ":/icons8_design_enabled_32px.png" );
    QPixmap design_disabled_icon( ":/icons8_design_disabled_32px.png" );
    QIcon design_icon( design_enabled_icon );
    design_icon.addPixmap( design_disabled_icon, QIcon::Disabled );
    mDesignAction = ui->toolBar->addAction(design_icon, "Design");
    mDesignAction->setEnabled(false);
    mDesignAction->setCheckable(true);
    connect(mDesignAction, &QAction::triggered, this, &MainWindow::showDesignPage);

    QWidget* empty = new QWidget();
    empty->setStyleSheet("background-color: none;");
    empty->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    ui->toolBar->addWidget(empty);

    mSettingsAction = ui->toolBar->addAction(QIcon(":/icons8_settings_3_32px.png"), "Settings");
    connect(mSettingsAction, &QAction::triggered, this, &MainWindow::showSettingsPage);

    QActionGroup *actiongrp = new QActionGroup(this);
    actiongrp->addAction(mHomeAction);
    actiongrp->addAction(mLiveAction);
    actiongrp->addAction(mReplayAction);
    actiongrp->addAction(mDesignAction);

    mHomeAction->setChecked(true);

    init();

    showHomePage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showHomePage()
{
    ui->stackedWidget->setCurrentWidget(ui->homeWidget);
}

void MainWindow::showLivePage()
{
    ui->dashboardWidget->board()->checkModification();

    mLiveAction->setChecked(true);
    ui->dashboardWidget->setMode(DashboardToolbar::dmLive);
    ui->dashboardWidget->setEdition(false);
    ui->stackedWidget->setCurrentWidget(ui->dashboardWidget);
}

void MainWindow::showDesignPage()
{
    ui->dashboardWidget->setMode(DashboardToolbar::dmLive);
    mDesignAction->setChecked(true);
    ui->dashboardWidget->setEdition(true);
    ui->stackedWidget->setCurrentWidget(ui->dashboardWidget);
}

void MainWindow::showReplayPage()
{
    mReplayAction->setChecked(true);
    ui->dashboardWidget->setEdition(false);
    ui->dashboardWidget->setMode(DashboardToolbar::dmReplay);
    ui->stackedWidget->setCurrentWidget(ui->dashboardWidget);
}

void MainWindow::showSettingsPage()
{
    QTBSettingsDialog dial(ui->dashboardWidget->board());
    dial.exec();
}

void MainWindow::fullScreen(bool fullscreen)
{
#ifdef __linux__
    setWindowState(windowState() ^ Qt::WindowFullScreen);
    if(fullscreen) {
        mNormalGeometry = geometry();
        ui->toolBar->hide();
    } else {
        setGeometry(mNormalGeometry);
        ui->toolBar->show();
    }
#elif _WIN32
    if(fullscreen) {
        mMaximized = isMaximized();
        setVisible(false);
        showFullScreen();
        ui->toolBar->hide();
        setVisible(true);
    } else {
        setVisible(false);
        if (mMaximized)
            showMaximized();
        else
            showNormal();
        ui->toolBar->show();
        setVisible(true);
    }
#endif

}

void MainWindow::projectSelected()
{
    mLiveAction->setEnabled(true);
    mDesignAction->setEnabled(true);
//    mReplayAction->setEnabled(true);
}

void MainWindow::init()
{
    connect(ui->dashboardWidget->project().data(), &QTBProject::loaded, this, &MainWindow::projectSelected);
    ui->homeWidget->setProject(ui->dashboardWidget->project());
    ui->homeWidget->loadWorkingDirectory();
}

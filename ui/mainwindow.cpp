#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/settings/settings_dialog.h"
#include "ui/board/factory/element_factory_helper.h"
#include "ui/widgets/utils/icon_factory.h"
#include "QSimpleUpdater.h"
#include <QPalette>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setContextMenuPolicy(Qt::NoContextMenu);
    connect(ui->homePage, SIGNAL(workingDirChanged()), this, SLOT(resetWorkingDir()));
    connect(ui->homePage, SIGNAL(showSettings()), this, SLOT(showSettings()));
    connect(ui->boardPage, SIGNAL(fullScreenMode()), this, SLOT(fullScreen()));
    connect(ui->boardPage->board(), SIGNAL(widgetClosed()), this, SLOT(close()));

    QIcon live_icon = create_icon(":/icons8_circled_play_enabled_32px.png");
    mLiveAction = ui->toolBar->addAction(live_icon, "  Live  ");
    mLiveAction->setCheckable(true);
    mLiveAction->setEnabled(false);
    connect(mLiveAction, &QAction::triggered, this, &MainWindow::showLivePage);

    QIcon playback_icon = create_icon(":/icons8_past_32px.png");
    mReplayAction = ui->toolBar->addAction(playback_icon, "Playback");
    mReplayAction->setCheckable(true);
    mReplayAction->setEnabled(false);
    connect(mReplayAction, &QAction::triggered, this, &MainWindow::showReplayPage);

    ui->toolBar->addSeparator();

    QIcon design_icon= create_icon(":/icons8_design_enabled_32px.png");
    mDesignAction = ui->toolBar->addAction(design_icon, " Design ");
    mDesignAction->setEnabled(false);
    mDesignAction->setCheckable(true);
    connect(mDesignAction, &QAction::triggered, this, &MainWindow::showDesignPage);

    QWidget* empty = new QWidget();
    empty->setStyleSheet("background-color: none;");
    empty->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    ui->toolBar->addWidget(empty);

    ui->toolBar->addSeparator();

    QIcon home_icon= create_icon(":/icons8_home_enabled_32px.png");
    mHomeAction = ui->toolBar->addAction(home_icon, "  Home  ");
    mHomeAction->setCheckable(true);
    connect(mHomeAction, &QAction::triggered, this, &MainWindow::showHomePage);

    QActionGroup *actiongrp = new QActionGroup(this);
    actiongrp->addAction(mLiveAction);
    actiongrp->addAction(mReplayAction);
    actiongrp->addAction(mHomeAction);

    mHomeAction->setChecked(true);

    mFullscreenShortcut = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(mFullscreenShortcut, &QShortcut::activated,
            this, &MainWindow::fullScreen);

    auto changeShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this);
    connect(changeShortcut, &QShortcut::activated, [=](){
        if(mLiveAction->isVisible() && mReplayAction->isVisible()) {
            if(!mHomeAction->isChecked()) {
                if(mLiveAction->isChecked()) {
                    mReplayAction->setChecked(true);
                    showReplayPage();
                } else {
                    mReplayAction->setChecked(false);
                    showLivePage();
                }
            }
        }
    });

    auto designShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    connect(designShortcut, &QShortcut::activated, [=](){
        if(mDesignAction->isChecked()) {
            mDesignAction->setChecked(false);
            showDesignPage(false);
        } else {
            mDesignAction->setChecked(true);
            showDesignPage(true);
        }
    });

    setWindowTitle(QString("CuteBoard"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showHomePage()
{
    if(ui->boardPage->mode() == BoardToolbar::dmLiveDesign ||
            ui->boardPage->mode() == BoardToolbar::dmReplayDesign)
        ui->boardPage->board()->checkModification();

    mDesignAction->setChecked(false);
    mDesignAction->setEnabled(false);
    ui->stackedWidget->setCurrentWidget(ui->homePage);
}

void MainWindow::showLivePage()
{
    mLiveAction->setChecked(true);
    mDesignAction->setChecked(false);
    mDesignAction->setEnabled(true);
    if(ui->boardPage->board()) {
        ui->boardPage->board()->setUserInteractions(false);
        ui->boardPage->board()->setMouseInteractions(false);
        ui->boardPage->board()->setStatMode(mStatMode);

        ui->boardPage->board()->startLiveTimer();
        ui->boardPage->board()->checkParameters();
        ui->boardPage->board()->loadHistoricalData();
        ui->boardPage->board()->replot(QCustomPlot::rpQueuedReplot);
        ui->boardPage->setMode(BoardToolbar::dmLive);
    }
    ui->stackedWidget->setCurrentWidget(ui->boardPage);
}

void MainWindow::showDesignPage(bool checked)
{
    ui->boardPage->board()->checkModification();
    if(checked) {
        if(mLiveAction->isChecked()) {
            ui->boardPage->setMode(BoardToolbar::dmLiveDesign);
        } else {
            ui->boardPage->setMode(BoardToolbar::dmReplayDesign);
        }
    } else {
        if(mLiveAction->isChecked()) {
            ui->boardPage->setMode(BoardToolbar::dmLive);
        } else {
            ui->boardPage->setMode(BoardToolbar::dmReplay);
        }
    }
}

void MainWindow::showReplayPage()
{
    mReplayAction->setChecked(true);
    mDesignAction->setChecked(false);
    mDesignAction->setEnabled(true);
    mStatMode = ui->boardPage->board()->statMode();

    ui->boardPage->board()->setUserInteractions(true);
    ui->boardPage->board()->setMouseInteractions(true);
    ui->boardPage->board()->setStatMode(false);
    ui->boardPage->board()->stopLiveTimer();
    ui->boardPage->board()->resetPlayback();
    ui->boardPage->board()->replot(QCustomPlot::rpQueuedReplot);
    ui->boardPage->setMode(BoardToolbar::dmReplay);
    ui->stackedWidget->setCurrentWidget(ui->boardPage);
}

void MainWindow::showSettings()
{
    SettingsDialog dial(ui->boardPage->board());
    dial.exec();
}

void MainWindow::fullScreen()
{
    if(isFullScreen()) {
        QGuiApplication::restoreOverrideCursor();
        ui->toolBar->show();
        ui->boardPage->toolBar()->show();

        setWindowFlag(Qt::FramelessWindowHint, false);
        setWindowFlag(Qt::WindowStaysOnTopHint, false);
        showMaximized();
    } else {
        ui->toolBar->hide();
        ui->boardPage->toolBar()->hide();

        setWindowFlag(Qt::FramelessWindowHint, true);
        setWindowFlag(Qt::WindowStaysOnTopHint, true);
        QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
        showFullScreen();
    }
}

void MainWindow::projectSelected()
{
    mLiveAction->setEnabled(true);
    mDesignAction->setEnabled(true);
    mReplayAction->setEnabled(true);

    setWindowTitle(QString("CuteBoard - %1 - %2").arg(ui->boardPage->board()->dataManager()->profileName())
                   .arg(ui->boardPage->project()->name()));

    if(mLiveAction->isVisible()) {
        showLivePage();
    } else if(mReplayAction->isVisible()) {
        showReplayPage();
    } else {
        mLiveAction->setChecked(true);
        showDesignPage(true);
    }
}

void MainWindow::resetWorkingDir()
{
    mLiveAction->setEnabled(false);
    mDesignAction->setEnabled(false);
    mReplayAction->setEnabled(false);

    setWindowTitle(QString("CuteBoard - %1").arg(ui->boardPage->board()->dataManager()->profileName()));
}

void MainWindow::checkUpdates()
{
    if(!mUpdateUrl.isEmpty()) {
        logger()->info() << QString("Checking updates from %1").arg(mUpdateUrl);
        mUpdater->checkForUpdates(mUpdateUrl);
    } else {
        logger()->warn() << QString("Can't check updates - no url");
    }
}

void MainWindow::init(QString profileName)
{
    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);

    mUpdateUrl = settings.value(QString("UpdateUrl")).toString();
    mUpdater  = QSimpleUpdater::getInstance();
    mUpdater->setModuleVersion(mUpdateUrl, QApplication::applicationVersion());
    mUpdater->setNotifyOnFinish(mUpdateUrl, false);
    mUpdater->setNotifyOnUpdate(mUpdateUrl, true);
    mUpdater->setDownloaderEnabled(mUpdateUrl, false);
    QTimer::singleShot(100, this, &MainWindow::checkUpdates);

    connect(&mUpdateTimer, &QTimer::timeout, this, &MainWindow::checkUpdates);
    int hours = 12*60*60*1000;
    mUpdateTimer.start(hours);

    int period = settings.value(QString("RefreshPeriod"),100).toInt();
    if(period < 50) {
        period = 50;
    }
    logger()->info() << QString("Board refresh period : %1ms").arg(period);
    ui->boardPage->board()->setRefreshTimerPeriod(period);

    if(!profileName.isEmpty())
        logger()->info() << QString("Requested profile from arguments : %1").arg(profileName);

    auto profiles = ui->boardPage->board()->dataManager()->profiles();
    if(profiles.count() > 0) {
        if(!profiles.contains(profileName)) {
            bool ok;
            QStringList listProfiles;
            for(auto i = profiles.begin(); i != profiles.end(); ++i) {
                if(i.value().isEmpty())
                    listProfiles.append(i.key());
                else
                    listProfiles.append(i.value());
            }

            listProfiles.sort();

            QString item = QInputDialog::getItem(this, "Data profile", "Select profile :", listProfiles, 0, false, &ok);


            if(ok && !item.isEmpty()) {
                if(!profiles.contains(item)) {
                    profileName = profiles.key(item);
                } else {
                    profileName = item;
                }
            } else {
                profileName.clear();
            }
        }
    } else {
        logger()->info() << QString("No available profile in data_sources.profiles");
        profileName.clear();
    }

    if(profileName.isEmpty()) {
        QMessageBox::warning(this, "Data Sources", "No available profile\nNo data source loaded.");
    } else {
        logger()->info() << QString("Loaded profile : %1").arg(profileName);
        ui->boardPage->board()->dataManager()->setProfileName(profileName);
        ui->boardPage->board()->initDataManager();
        logger()->info() << QString("DataManager ready");

        if(ui->boardPage->board()->dataManager()->playbackDataSources().count() == 0) {
            mReplayAction->setVisible(false);
        } else {
            mReplayAction->setVisible(true);
        }

        if(ui->boardPage->board()->dataManager()->liveDataSources().count() == 0) {
            mLiveAction->setVisible(false);
        } else {
            mLiveAction->setVisible(true);
        }
    }

    auto project = ui->boardPage->project();
    if(project) {
        connect(project.data(), &Project::loaded, this, &MainWindow::projectSelected);
        ui->homePage->setProject(project);
        ui->homePage->loadUserWorkingDirectory();

        ui->homePage->setProfileWorkingDir(ui->boardPage->board()->dataManager()->profileName(),
                                           ui->boardPage->board()->dataManager()->profileWorkingDirectory());
    }

    showHomePage();

    setWindowTitle(QString("CuteBoard - %1").arg(ui->boardPage->board()->dataManager()->profileName()));
}

bool MainWindow::loadProject(QString projectPath, QString page)
{
    auto projectDir = QDir(projectPath);
    auto projectName = projectDir.dirName();
    QString userWorkingDir;
    if(projectDir.cdUp()) {
        userWorkingDir = projectDir.absolutePath();
    }
    if(!projectName.isEmpty() && !userWorkingDir.isEmpty()) {
        ui->homePage->setUserWorkingDir(userWorkingDir);
        ui->homePage->loadUserProject(projectName);
        auto project = ui->boardPage->project();
        if(project && !page.isEmpty()) {
            project->requestPage(page);
        }
        return true;
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    ui->boardPage->board()->checkModification();
    event->accept();
}

QSize MainWindow::minimumSizeHint() const
{
    return QSize(0,0);
}

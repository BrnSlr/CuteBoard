#include "board_toolbar.h"
#include "ui_board_toolbar.h"
#include "widgets/find_param_widget.h"
#include "ui/widgets/utils/icon_factory.h"

#include <QCollator>

BoardToolbar::BoardToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoardToolbar),
    mMode(dmLive),
    mStartTimeMsecs(-1),
    mEndTimeMsecs(-1),
    mCurrentSliderValue(-1),
    mReplayInterval(100)
{
    ui->setupUi(this);
    mComboboxEventFilter.setWatchedCombobox(ui->comboBox);

    mPageIcon = create_icon(":/icons8_file_50px.png");
    mRightClickMenu = new QMenu(this);

    QActionGroup *group = new QActionGroup(this);

    mSmallAction = new QAction("Small toolbar", this);
    mSmallAction->setCheckable(true);
    mSmallAction->setChecked(true);
    mMediumAction = new QAction("Medium toolbar", this);
    mMediumAction->setCheckable(true);
    mLargeAction = new QAction("Large toolbar", this);
    mLargeAction->setCheckable(true);

    mCurrentAction = mSmallAction;

    group->addAction(mSmallAction);
    group->addAction(mMediumAction);
    group->addAction(mLargeAction);

    mRightClickMenu->addAction(mSmallAction);
    mRightClickMenu->addAction(mMediumAction);
    mRightClickMenu->addAction(mLargeAction);

    connect(mLargeAction, &QAction::triggered, [=](){
        if(mCurrentAction != mLargeAction) {
            double factor = 1.;
            if(mCurrentAction == mSmallAction) {
                factor = 1.4 * 1.4;
            }
            if(mCurrentAction == mMediumAction) {
                factor = 1.4;
            }
            changeToolbarSize(factor);
            mCurrentAction = mLargeAction;
        }
    });
    connect(mMediumAction, &QAction::triggered, [=](){
        if(mCurrentAction != mMediumAction) {
            double factor = 1.;
            if(mCurrentAction == mSmallAction) {
                factor = 1.4;
            }
            if(mCurrentAction == mLargeAction) {
                factor = 1./1.4;
            }
            changeToolbarSize(factor);
            mCurrentAction = mMediumAction;
        }
    });
    connect(mSmallAction, &QAction::triggered, [=](){
        if(mCurrentAction != mSmallAction) {
            double factor = 1.;
            if(mCurrentAction == mLargeAction) {
                factor = 1./(1.4 * 1.4);
            }
            if(mCurrentAction == mMediumAction) {
                factor = 1./1.4;
            }
            changeToolbarSize(factor);
            mCurrentAction = mSmallAction;
        }
    });

    auto textColor = QApplication::palette().toolTipText().color();

    ui->sliderPlot->yAxis->setVisible(false);
    ui->sliderPlot->xAxis->setBasePen(Qt::NoPen);
    ui->sliderPlot->xAxis->setTickPen(textColor);
    ui->sliderPlot->xAxis->setSubTickPen(Qt::NoPen);
    ui->sliderPlot->xAxis->setTickLabelColor(textColor);
    ui->sliderPlot->xAxis->grid()->setVisible(false);
    ui->sliderPlot->xAxis->setTickLabelSide(QCPAxis::lsOutside);
    ui->sliderPlot->axisRect()->setMinimumMargins(QMargins(0,0,0,12));
    ui->sliderPlot->axisRect()->setMargins(QMargins(0,0,0,12));
    ui->sliderPlot->axisRect()->setMinimumSize(0,0);
    ui->sliderPlot->axisRect()->setBackground(QApplication::palette().window());
    ui->sliderPlot->setBackground(QApplication::palette().window());

    mTimeTicker = QSharedPointer<QCPAxisTickerTime>(new QCPAxisTickerTime);
    mTimeTicker->setTimeFormat("%h:%m:%s");
    mTimeTicker->setTickCount(5);
    ui->sliderPlot->xAxis->setTicker(mTimeTicker);

    mCalendar = new CalendarWidget(this);
    ui->dateEdit->setCalendarWidget(mCalendar);

    QIcon actionUp_icon = create_icon(":/icons8_collapse_arrow_filled_50px.png");
    QAction *actionUp = new QAction(actionUp_icon, QString(""));

    QList<QKeySequence> shortcutsPreviousPage;
    shortcutsPreviousPage << QKeySequence::MoveToPreviousPage << QKeySequence(Qt::Key_Minus);
    actionUp->setShortcuts(shortcutsPreviousPage);
    actionUp->setToolTip("Move to previous page [PgUp][-]");

    ui->upButton->setDefaultAction(actionUp);
    connect(ui->upButton, &QToolButton::triggered,
            [this]() {
        if(ui->comboBox->currentIndex() > 0) {
            ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex() - 1);
        }
    });

    QIcon actionDown_icon = create_icon(":/icons8_expand_arrow_filled_50px.png");
    QAction *actionDown = new QAction(actionDown_icon, QString(""));

    QList<QKeySequence> shortcutsNextPage;
    shortcutsNextPage << QKeySequence::MoveToNextPage << QKeySequence(Qt::Key_Plus);
    actionDown->setShortcuts(shortcutsNextPage);
    actionDown->setToolTip("Move to next page [PgDown][+]");

    ui->downButton->setDefaultAction(actionDown);
    connect(ui->downButton, &QToolButton::triggered,
            [this]() {
        if(ui->comboBox->currentIndex() < ui->comboBox->count()-1) {
            ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex() + 1);
        }
    });

    QAction *actionReload = new QAction(QString(""));
    QIcon icoReload = create_icon(":/icons8_refresh_filled_50px.png");
    actionReload->setIcon(icoReload);
    actionReload->setToolTip("Reload");
    QList<QKeySequence> shortcutsReload;
    shortcutsReload << QKeySequence(Qt::Key_F5);
    actionReload->setShortcuts(shortcutsReload);
    ui->reloadButton->setDefaultAction(actionReload);
    connect(actionReload, &QAction::triggered,
            this, &BoardToolbar::reload);

    QAction *actionStat = new QAction(QString(""));
    QIcon icoStat = create_icon(":/icons8_plus_minus_2_50px.png");
    actionStat->setIcon(icoStat);
    actionStat->setCheckable(true);
    actionStat->setChecked(false);
    actionStat->setToolTip("Statistics - Min/Max");
    ui->statButton->setDefaultAction(actionStat);
    connect(actionStat, SIGNAL(toggled(bool)),
            this,SIGNAL(enableStat(bool)));

    QAction *actionPause = new QAction(QString(""));
    QIcon icoPause= create_icon(":/icons8_hand_50px_2.png");
    actionPause->setIcon(icoPause);
    actionPause->setShortcut(QKeySequence(tr("Ctrl+Space")));
    actionPause->setCheckable(true);
    actionPause->setChecked(false);
    actionPause->setToolTip("Pause/Play [Ctrl+Space]");
    ui->pauseButton->setDefaultAction(actionPause);
    connect(actionPause, SIGNAL(toggled(bool)),
            this,SIGNAL(pagePaused(bool)));

    connect(actionPause, &QAction::toggled, [=](const bool toggled){
        emit pagePaused(toggled);
        ui->timeLiveButton->setBlinking(toggled);
    });

    //    QAction *actionInteraction = new QAction(QIcon(":/icons8_nui2_filled_50px.png"), QString(""));
    //    actionInteraction->setCheckable(true);
    //    actionInteraction->setChecked(false);
    //    actionInteraction->setToolTip("Enable/Disable plots tracer");
    //    ui->interactionButton->setDefaultAction(actionInteraction);
    //    ui->interactionButton->setVisible(false);
    //    connect(actionInteraction, SIGNAL(toggled(bool)),
    //            this,SIGNAL(enableInteraction(bool)));

    //    QAction *fullscreenAction = new QAction(QIcon(":/icons8_full_screen_32px.png"), QString(""));
    //    fullscreenAction->setCheckable(true);
    //    fullscreenAction->setShortcut(QKeySequence(QKeySequence::FullScreen));
    //    fullscreenAction->setChecked(false);
    //    fullscreenAction->setToolTip("Fullscreen");
    //    ui->fullscreenButton->setDefaultAction(fullscreenAction);
    //    connect(ui->fullscreenButton, SIGNAL(triggered(QAction *)),
    //            this,SIGNAL(fullScreen()));

    QIcon icoScr= create_icon(":/icons8_screenshot_50px.png");
    QAction *screenShotAction = new QAction(icoScr, QString(""));
    screenShotAction->setCheckable(false);
    screenShotAction->setToolTip("Screenshot to Images Dir [Ctrl+P]");

    screenShotAction->setShortcut(QKeySequence(tr("Ctrl+p")));

    ui->printButton->setDefaultAction(screenShotAction);
    ui->print2Button->setDefaultAction(screenShotAction);
    connect(screenShotAction, SIGNAL(triggered()),
            this,SIGNAL(printScreen()));

    QIcon icoFind= create_icon(":/icons8_search_filled_50px.png");
    QAction *findAction = new QAction(icoFind, QString(""));
    findAction->setCheckable(false);
    findAction->setShortcut(QKeySequence(tr("Ctrl+f")));
    findAction->setToolTip("Find parameter [Ctrl+F]");
    ui->findButton->setDefaultAction(findAction);
    ui->find2button->setDefaultAction(findAction);
    connect(findAction, &QAction::triggered, [=]() {
        find_param_widget w;
        w.setProject(mProject);
        connect(&w, &find_param_widget::pageRequested,
                this, &BoardToolbar::pageRequested);
        w.exec();
    });

    mReplayTimer = new QTimer(this);
    mReplayTimer->setTimerType(Qt::PreciseTimer);
    mReplayTimer->setInterval(100);
    connect(mReplayTimer, &QTimer::timeout, this, &BoardToolbar::play);

    QIcon icoPlay= create_icon(":/icons8_start_50px.png");
    mPlayAction = new QAction(icoPlay, QString(""));
    mPlayAction->setCheckable(true);
    connect(mPlayAction, &QAction::triggered, this, &BoardToolbar::startReplay);
    ui->playToolButton->setDefaultAction(mPlayAction);

    QIcon icoStop = create_icon(":/icons8_stop_squared_50px.png");
    QAction *actionStop = new QAction(icoStop, QString(""));
    connect(actionStop, &QAction::triggered, this, &BoardToolbar::stopReplay);
    ui->stopToolButton->setDefaultAction(actionStop);

    QIcon icoSpeed = create_icon(":/icons8_quick_mode_on_50px.png");
    QAction *actionSpeed = new QAction(icoSpeed, QString(""));
    actionSpeed->setCheckable(true);
    connect(actionSpeed, &QAction::triggered, [=]() {
        if(mReplayInterval == 100)
            mReplayInterval = 1000;
        else
            mReplayInterval = 100;
    });

    ui->speedToolButton->setDefaultAction(actionSpeed);


    ui->chronoWidget->setVisible(false);

    QAction *actionChrono = new QAction(QString(""));
    QIcon icoChrono= create_icon(":/icons8_stopwatch_50px_1.png");
    actionChrono->setIcon(icoChrono);
    actionChrono->setCheckable(true);
    actionChrono->setChecked(false);
    ui->chronoButton->setDefaultAction(actionChrono);

    connect(actionChrono, &QAction::toggled, [=](const bool toggled){
        ui->chronoWidget->setVisible(toggled);
    });

    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SIGNAL(pageRequested(const QString)));
    ui->comboBox->setCurrentIndex(-1);

    setDate(QDate::currentDate());
    ui->replaySlider->setTracking(false);
    ui->replaySlider->setEnabled(false);
    ui->timeEdit->setEnabled(false);
    ui->playToolButton->setEnabled(false);
    ui->stopToolButton->setEnabled(false);
    ui->speedToolButton->setEnabled(false);
    ui->dateEdit->setEnabled(true);

    //    ui->comboBox->view()->setFixedWidth(400);
}

BoardToolbar::~BoardToolbar()
{
    delete ui;
}

void BoardToolbar::setProject(const QSharedPointer<Project> &project)
{
    mProject = project;

    connect(this, &BoardToolbar::pageRequested,
            mProject.data(), &Project::requestPage);

    connect(mProject.data(), &Project::loaded,
            this, &BoardToolbar::updateList);

    connect(mProject.data(), &Project::updated,
            this, &BoardToolbar::updateList);

    connect(mProject.data(), &Project::pagesListUpdated,
            this, &BoardToolbar::updateList);

    connect(mProject.data(), &Project::pageLoaded,
            this, &BoardToolbar::pageSelected);
}

void BoardToolbar::loadPage(const QString& pageName)
{
    if(mProject) {
        auto page = mProject->page(pageName);
        if(page) {
            ui->comboBox->setCurrentText(pageName);
            return;
        }
    }
    ui->comboBox->setCurrentIndex(-1);
}

void BoardToolbar::setLiveDateTime(const QDateTime& time)
{
    ui->timeLiveButton->setDateTime(time);
}

void BoardToolbar::loadFirstPage()
{
    if(ui->comboBox->currentIndex() >= 0)
        return;
    ui->comboBox->setCurrentIndex(0);
}

void BoardToolbar::setMode(BoardToolbar::DashboardMode mode)
{
    mMode = mode;

    switch(mMode) {
    case dmLive:
        ui->stackedWidget->setCurrentWidget(ui->pageLive);
        ui->sliderWidget->setVisible(false);
        ui->pagesNavWidget->setVisible(true);
        ui->liveButtonsWidget->setVisible(true);
        mReplayTimer->stop();
        mPlayAction->setChecked(false);
        break;
    case dmReplay:
        ui->stackedWidget->setCurrentWidget(ui->pageReplay);
        ui->sliderWidget->setVisible(true);
        ui->pagesNavWidget->setVisible(true);
        ui->liveButtonsWidget->setVisible(false);
        mReplayTimer->stop();
        mPlayAction->setChecked(false);
        break;
    case dmLiveDesign:
        ui->stackedWidget->setCurrentWidget(ui->pageLive);
        ui->sliderWidget->setVisible(false);
        ui->pagesNavWidget->setVisible(false);
        ui->liveButtonsWidget->setVisible(false);
        break;
    case dmReplayDesign:
        ui->stackedWidget->setCurrentWidget(ui->pageReplay);
        ui->sliderWidget->setVisible(true);
        ui->pagesNavWidget->setVisible(false);
        ui->liveButtonsWidget->setVisible(false);
        break;
    }
    ui->sliderPlot->replot();
}

void BoardToolbar::updateList()
{
    const QSignalBlocker blocker(this);
    if(mProject) {
        ui->comboBox->clear();
        QStringList pagesNames = mProject->pages().keys();

        QCollator collator;
        collator.setNumericMode(true);

        std::sort(pagesNames.begin(),
                  pagesNames.end(),
                  collator);

        for (int i = 0; i < pagesNames.size(); ++i) {
            ui->comboBox->addItem(mPageIcon,pagesNames.at(i));
        }

        loadPage(mProject->currentPageName());
    }
}

void BoardToolbar::pageSelected()
{    
    const QSignalBlocker blocker(this);
    if(mProject) {
        loadPage(mProject->currentPageName());
    }
}

void BoardToolbar::chrono()
{
    if(!mChronometer.isVisible())
        mChronometer.show();
    else
        mChronometer.hide();
}

void BoardToolbar::startReplay()
{
    if(mReplayTimer->isActive()) {
        stopReplay();
    } else {
        mReplayTimer->start();
    }
}

void BoardToolbar::stopReplay()
{
    mReplayTimer->stop();
    mPlayAction->setChecked(false);
}

void BoardToolbar::play()
{
    auto value = ui->replaySlider->value();
    ui->replaySlider->setValue(value + mReplayInterval);
}

void BoardToolbar::on_replaySlider_valueChanged(int value)
{

    const QSignalBlocker blocker(ui->timeEdit);
    ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(value));

    mCurrentSliderValue = value;
    emit timeUpdate(mCurrentSliderValue, mDate);
}

QDate BoardToolbar::date() const
{
    return mDate;
}

void BoardToolbar::setDate(const QDate &date)
{
    mDate = date;
    ui->dateEdit->setDate(date);
}

int BoardToolbar::endTimeMsecs() const
{
    return mEndTimeMsecs;
}

void BoardToolbar::setEndTimeMsecs(int endTimeMsecs)
{
    int sliderValue = ui->replaySlider->value();
    const QSignalBlocker blocker(ui->replaySlider);
    mEndTimeMsecs = endTimeMsecs;
    ui->labelEnd->setText(QTime::fromMSecsSinceStartOfDay(endTimeMsecs).toString("hh:mm:ss.zzz"));
    ui->replaySlider->setRange(mStartTimeMsecs, mEndTimeMsecs);
    ui->replaySlider->setSingleStep(100);
    ui->replaySlider->setPageStep(500);

    ui->sliderPlot->xAxis->setRange(mStartTimeMsecs / 1000., mEndTimeMsecs / 1000.);
    ui->sliderPlot->replot();

    if(mEndTimeMsecs >= 0 && mStartTimeMsecs >= 0) {
        ui->replaySlider->setEnabled(true);
        ui->timeEdit->setEnabled(true);
        ui->dateEdit->setEnabled(true);
        ui->playToolButton->setEnabled(true);
        ui->stopToolButton->setEnabled(true);
        ui->speedToolButton->setEnabled(true);
    }

    if(mCurrentSliderValue == -1) {
        const QSignalBlocker blocker(ui->replaySlider);
        ui->replaySlider->setValue(mEndTimeMsecs);
        const QSignalBlocker blocker2(ui->timeEdit);
        ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(mEndTimeMsecs));
        emit timeUpdate(ui->replaySlider->value(), mDate);
    } else if(ui->replaySlider->value() != sliderValue) {
        const QSignalBlocker blocker2(ui->timeEdit);
        ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(ui->replaySlider->value()));

        if(mCurrentSliderValue != -1)
            mCurrentSliderValue = ui->replaySlider->value();
        emit timeUpdate(ui->replaySlider->value(), mDate);
    }
}

int BoardToolbar::startTimeMsecs() const
{
    return mStartTimeMsecs;
}

void BoardToolbar::setStartTimeMsecs(int startTimeMsecs)
{
    int sliderValue = ui->replaySlider->value();
    const QSignalBlocker blocker(ui->replaySlider);
    if(startTimeMsecs != mStartTimeMsecs) {
        mStartTimeMsecs = startTimeMsecs;
        ui->labelStart->setText(QTime::fromMSecsSinceStartOfDay(startTimeMsecs).toString("hh:mm:ss.zzz"));

        if(ui->replaySlider->value() > startTimeMsecs) {
            ui->replaySlider->setValue(mStartTimeMsecs);
            ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(mStartTimeMsecs));
            emit timeUpdate(mStartTimeMsecs, mDate);
        }

        ui->replaySlider->setRange(mStartTimeMsecs, mEndTimeMsecs);
        ui->replaySlider->setSingleStep(100);
        ui->replaySlider->setPageStep(500);

        ui->sliderPlot->xAxis->setRange(mStartTimeMsecs / 1000., mEndTimeMsecs / 1000.);
        ui->sliderPlot->replot();

        if(mEndTimeMsecs >= 0 && mStartTimeMsecs >= 0) {
            ui->replaySlider->setEnabled(true);
            ui->timeEdit->setEnabled(true);
            ui->dateEdit->setEnabled(true);
            ui->playToolButton->setEnabled(true);
            ui->stopToolButton->setEnabled(true);
            ui->speedToolButton->setEnabled(true);
        }

        if(ui->replaySlider->value() != sliderValue) {
            const QSignalBlocker blocker2(ui->timeEdit);
            ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(ui->replaySlider->value()));

            if(mCurrentSliderValue != -1)
                mCurrentSliderValue = ui->replaySlider->value();

            emit timeUpdate(ui->replaySlider->value(), mDate);
        }
    }
}

void BoardToolbar::on_timeEdit_timeChanged(const QTime &time)
{
    int value = time.msecsSinceStartOfDay();
    {
        const QSignalBlocker blocker(ui->replaySlider);
        ui->replaySlider->setValue(value);
    }
    mCurrentSliderValue = value;
    emit timeUpdate(mCurrentSliderValue, mDate);
}

void BoardToolbar::on_replaySlider_sliderMoved(int position)
{
    const QSignalBlocker blocker(ui->timeEdit);
    ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(position));
}

int BoardToolbar::currentSliderValue() const
{
    return ui->replaySlider->value();
}

void BoardToolbar::contextMenuEvent(QContextMenuEvent *event)
{
    mRightClickMenu->exec(mapToGlobal(event->pos()));
}

void BoardToolbar::changeToolbarSize(double factor)
{
    auto l = findChildren<QToolButton*>();
    for(auto b : l) {
        auto f = b->font();
        f.setPointSizeF(factor*f.pointSizeF());
        b->setFont(f);

        auto s = b->iconSize();
        s *= factor;
        b->setIconSize(s);
    }

    auto ll = findChildren<QDateEdit*>();
    for(auto b : ll) {
        auto f = b->font();
        f.setPointSizeF(factor*f.pointSizeF());
        b->setFont(f);
    }

    auto lll = findChildren<QTimeEdit*>();
    for(auto b : lll) {
        auto f = b->font();
        f.setPointSizeF(factor*f.pointSizeF());
        b->setFont(f);
    }

    auto llll = findChildren<QComboBox*>();
    for(auto b : llll) {
        auto f = b->font();
        f.setPointSizeF(factor*f.pointSizeF());
        b->setFont(f);

        auto s = b->iconSize();
        s *= factor;
        b->setIconSize(s);
    }
}

BoardToolbar::DashboardMode BoardToolbar::mode() const
{
    return mMode;
}

void BoardToolbar::setDataManager(const QSharedPointer<DataManager> &dataManager)
{
    mDataManager = dataManager;
    bool okStart, okEnd;
    auto start = mDataManager->playbackStartTime(okStart);
    auto end = mDataManager->playbackEndTime(okEnd);
    if(okStart && okEnd) {
        ui->replaySlider->setEnabled(true);
        ui->timeEdit->setEnabled(true);
        ui->playToolButton->setEnabled(true);
        ui->stopToolButton->setEnabled(true);
        ui->speedToolButton->setEnabled(true);
        setStartTimeMsecs(start);
        setEndTimeMsecs(end);
    } else {
        ui->replaySlider->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        ui->playToolButton->setEnabled(false);
        ui->stopToolButton->setEnabled(false);
        ui->speedToolButton->setEnabled(false);
        mReplayTimer->stop();
        mPlayAction->setChecked(false);
    }
    connect(mDataManager.data(), &DataManager::playbackTimeUpdated,[=]() {
        bool okStart, okEnd;
        auto start = mDataManager->playbackStartTime(okStart);
        auto end = mDataManager->playbackEndTime(okEnd);
        if(okStart && okEnd) {
            setStartTimeMsecs(start);
            setEndTimeMsecs(end);
            ui->replaySlider->setEnabled(true);
            ui->timeEdit->setEnabled(true);
            ui->playToolButton->setEnabled(true);
            ui->stopToolButton->setEnabled(true);
            ui->speedToolButton->setEnabled(true);
        } else {
            ui->replaySlider->setValue(-1);
            ui->labelStart->setText("--:--:--.---");
            ui->labelEnd->setText("--:--:--.---");
            ui->replaySlider->setEnabled(false);
            ui->timeEdit->setEnabled(false);
            ui->playToolButton->setEnabled(false);
            ui->stopToolButton->setEnabled(false);
            ui->speedToolButton->setEnabled(false);
            mReplayTimer->stop();
            mPlayAction->setChecked(false);
        }
    });
    connect(mDataManager.data(), &DataManager::playbackValidDatesUpdated,[=]() {
        auto dates = mDataManager->playbackValidDates();
        auto  cal = qobject_cast<CalendarWidget *>(ui->dateEdit->calendarWidget());
        if(cal) {
            for(auto date : dates) {
                cal->addValidDate(date);
            }
        }
    });
}

void BoardToolbar::on_dateEdit_userDateChanged(const QDate &date)
{
    mDate = date;
    emit timeUpdate(mCurrentSliderValue, mDate);
}


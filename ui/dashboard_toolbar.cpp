#include "dashboard_toolbar.h"
#include "ui_dashboard_toolbar.h"

DashboardToolbar::DashboardToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardToolbar)
{
    ui->setupUi(this);

    QAction *actionUp = new QAction(QIcon(":/icons8_up4_16px.png"), QString(""));
    actionUp->setShortcut(QKeySequence::MoveToPreviousPage);
    ui->upButton->setDefaultAction(actionUp);
    connect(ui->upButton, &QToolButton::triggered,
            [this]() {
        ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex() - 1);
        if(ui->comboBox->currentIndex() == -1)
            ui->comboBox->setCurrentIndex(0);
    });

    QAction *actionDown = new QAction(QIcon(":/icons8_down4_16px.png"), QString(""));
    actionDown->setShortcut(QKeySequence::MoveToNextPage);
    ui->downButton->setDefaultAction(actionDown);
    connect(ui->downButton, &QToolButton::triggered,
            [this]() {
        ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex() + 1);
        if(ui->comboBox->currentIndex() == -1)
            ui->comboBox->setCurrentIndex(ui->comboBox->count() - 1 );

    });

    QAction *actionFullScreen = new QAction(QIcon(":/icons8_full_screen_32px.png"), QString(""));
    actionFullScreen->setShortcut(QKeySequence::FullScreen);
    actionFullScreen->setCheckable(true);
    actionFullScreen->setChecked(false);
    ui->fullScreenButton->setDefaultAction(actionFullScreen);
    connect(actionFullScreen, SIGNAL(toggled(bool)),
            this,SIGNAL(fullScreen(bool)));

    QAction *actionPause = new QAction(QIcon(":/icons8_pause_32px.png"), QString(""));
    actionPause->setShortcut(QKeySequence(tr("Ctrl+Space")));
    actionPause->setCheckable(true);
    actionPause->setChecked(false);
    ui->pauseButton->setDefaultAction(actionPause);
    connect(actionPause, SIGNAL(toggled(bool)),
            this,SIGNAL(pagePaused(bool)));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SIGNAL(pageIndexChanged(const QString)));

    ui->comboBox->setCurrentIndex(-1);
}

DashboardToolbar::~DashboardToolbar()
{
    delete ui;
}

void DashboardToolbar::setProject(const QSharedPointer<QTBProject> &project)
{
    mProject = project;
}

void DashboardToolbar::loadPage(const QString& pageName)
{
    if(mProject) {
        QTBPage *page = mProject->page(pageName);
        if(page) {
            ui->comboBox->setCurrentText(pageName);
            return;
        }
    }
    ui->comboBox->setCurrentIndex(-1);
}

void DashboardToolbar::setTime(const QString& time)
{
    ui->timeLabel->setText(time);
}

void DashboardToolbar::loadFirstPage()
{
    if(ui->comboBox->currentIndex() >= 0)
        return;
    ui->comboBox->setCurrentIndex(0);
}

void DashboardToolbar::setEdition(bool edition)
{
    ui->pagesNavWidget->setVisible(!edition);
    ui->fullScreenButton->setVisible(!edition);
}

void DashboardToolbar::updateList()
{
    const QSignalBlocker blocker(this);
    if(mProject) {
        ui->comboBox->clear();
        QStringList pagesNames = mProject->pages().keys();
        pagesNames.sort();
        for (int i = 0; i < pagesNames.size(); ++i) {
            ui->comboBox->addItem(QIcon(QPixmap(":/icons8_file_50px.png")),pagesNames.at(i));
        }

        loadPage(mProject->currentPageName());
    }
}

void DashboardToolbar::pageSelected()
{    
    const QSignalBlocker blocker(this);
    if(mProject) {
        loadPage(mProject->currentPageName());
    }
}

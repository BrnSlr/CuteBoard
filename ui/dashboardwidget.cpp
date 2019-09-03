#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"

DashboardWidget::DashboardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardWidget)
{
    ui->setupUi(this);
    ui->hsplitter->setStretchFactor(0,0);
    ui->hsplitter->setStretchFactor(1,1);
    setAcceptDrops(true);

    mBoard = ui->dashboard;
    mBoard->initDataManager();

    ui->liveToolbar->setProject(mBoard->project());
    ui->pagePicker->setProject(mBoard->project());
    ui->propertiesPicker->setProject(mBoard->project());
    ui->parameterPicker->setDataManager(mBoard->dataManager());

    connect(ui->liveToolbar, &DashboardToolbar::pageIndexChanged, mBoard->project().data(), &QTBProject::requestPage);
    connect(ui->liveToolbar, SIGNAL(fullScreen(bool)), this, SIGNAL(fullScreenMode(bool)));

    connect(ui->liveToolbar, &DashboardToolbar::pagePaused, [this](bool refreshDisabled) {
        mBoard->setLiveDataRefreshEnabled(!refreshDisabled);
    });

    connect(ui->pagePicker, &PagePickerWidget::saveCurrentPage, mBoard, &QTBoard::savePage);
    connect(ui->pagePicker, &PagePickerWidget::openPage, mBoard->project().data(), &QTBProject::requestPage);

    connect(mBoard->dataManager().data(), &QTBDataManager::parametersUpdated, ui->parameterPicker, &ParameterPickerWidget::updateParameterList);

    connect(mBoard->project().data(), &QTBProject::loaded, ui->liveToolbar, &DashboardToolbar::updateList);
    connect(mBoard->project().data(), &QTBProject::loaded, ui->pagePicker, &PagePickerWidget::updateList);
    connect(mBoard->project().data(), &QTBProject::loaded, ui->propertiesPicker, &PropertiesPickerWidget::updateList);

    connect(mBoard->project().data(), &QTBProject::pagesListUpdated, ui->liveToolbar, &DashboardToolbar::updateList);
    connect(mBoard->project().data(), &QTBProject::pagesListUpdated, ui->pagePicker, &PagePickerWidget::updateList);

    connect(mBoard->project().data(), &QTBProject::pageLoaded, ui->liveToolbar, &DashboardToolbar::pageSelected);
    connect(mBoard->project().data(), &QTBProject::pageLoaded, ui->pagePicker, &PagePickerWidget::pageSelected);

    connect(mBoard, &QTBoard::timeUpdate, this, &DashboardWidget::updateTime);
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}


void DashboardWidget::setEdition(bool edit)
{
    ui->editionWidget->setVisible(edit);
    if(!edit)
        ui->liveToolbar->loadFirstPage();
    ui->liveToolbar->setEdition(edit);
    mBoard->dashboardLayout()->setLocked(!edit);
}

void DashboardWidget::updateTime(const QDateTime& time)
{
    if(mBoard->liveDataRefreshEnabled()) {
        QString timeString = time.toString("HH:mm:ss.zzz ") + time.timeZone().displayName(QTimeZone::DaylightTime,QTimeZone::OffsetName);
        ui->liveToolbar->setTime(timeString);
    }
}

QTBoard *DashboardWidget::board() const
{
    return mBoard;
}

QSharedPointer<QTBProject> DashboardWidget::project() const
{
    return mBoard->project();
}

void DashboardWidget::savePage()
{
    mBoard->savePage();
}

#include "board_page.h"
#include "ui_board_page.h"

BoardPage::BoardPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoardPage),
    mBoardWidget(nullptr),
    mToolBar(nullptr)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_KeyCompression, true);
    setFocusPolicy(Qt::StrongFocus);

    mKeyboardTimer = new QTimer(this);
    mKeyboardTimer->setSingleShot(true);
    mKeyboardTimer->setInterval(250);

    connect(mKeyboardTimer, &QTimer::timeout,
            this, &BoardPage::keyboardText);

    ui->hsplitter->setStretchFactor(0,0);
    ui->hsplitter->setStretchFactor(1,1);
    setAcceptDrops(true);

    mToolBar = new BoardToolbar();
    mBoardWidget = new BoardWidget();
    mBoard = mBoardWidget->board();

    ui->verticalLayoutBoard->addWidget(mToolBar, 0);
    ui->verticalLayoutBoard->addWidget(mBoardWidget, 1);

    mToolBar->setProject(mBoard->project());
    ui->pagePicker->setBoard(mBoard);

    ui->propertiesPicker->setProject(mBoard->project());
    ui->parameterPicker->setDataManager(mBoard->dataManager());
    ui->parameterPicker->setProject(mBoard->project());

    mToolBar->setDataManager(mBoard->dataManager());

    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);
    bool clock = settings.value(QString("DataRefClock"), true).toBool();
    mBoard->dataManager()->setUseDataSourceAsReferenceClock(clock);

    ui->tabWidget_4->setCurrentIndex(0);

    connect(mToolBar, &BoardToolbar::pagePaused, [this](bool refreshDisabled) {
        mBoard->setLiveDataRefreshEnabled(!refreshDisabled);
    });

    connect(mToolBar, &BoardToolbar::enableStat, [this](bool enableStat) {
        mBoard->setStatMode(enableStat);
    });

    connect(mToolBar, &BoardToolbar::enableInteraction, [this](bool enableInteraction) {
        mBoard->setMouseInteractions(enableInteraction);
    });

    connect(mToolBar, &BoardToolbar::printScreen,
            this, &BoardPage::saveJpg);

    connect(mToolBar, &BoardToolbar::fullScreen,
            this, &BoardPage::fullScreenMode);

    connect(mToolBar, &BoardToolbar::reload,
            mBoard, &Board::reloadProject);

    connect(mBoard, &Board::timeUpdate,
            this, &BoardPage::updateTime);

    connect(mToolBar, &BoardToolbar::timeUpdate, mBoard,
            &Board::updatePlayback);

    mBoard->setReferencePlaybackTime(mToolBar->currentSliderValue());
    mBoard->setReferencePlaybackDate(mToolBar->date());
}

BoardPage::~BoardPage()
{  
    delete ui;
}

void BoardPage::updateTime(const QDateTime& time)
{
    if(mBoard->liveDataRefreshEnabled()) {
        auto msec = time.toMSecsSinceEpoch();
        msec = (msec/100) * 100;
        mToolBar->setLiveDateTime(QDateTime::fromMSecsSinceEpoch(msec).toUTC());
    }
}

void BoardPage::saveJpg()
{
    auto pixToolbar = QPixmap(mToolBar->size());
    mToolBar->render(&pixToolbar);

    auto pixBoard = mBoard->toPixmap();

    int w = pixToolbar.width();
    if(pixBoard.width() > w)
        w = pixBoard.width();

    int h = pixToolbar.height() + pixBoard.height();

    QPixmap result(w,h);
    QPainter painter(&result);
    painter.drawPixmap(0,0,pixToolbar);
    painter.drawPixmap(0,pixToolbar.height(), pixBoard);

    auto time = QDateTime::currentDateTimeUtc();
    auto buffer = result.toImage();

    auto picPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    auto picDir = QDir(picPath + QDir::separator() + "CuteBoard");
    if(!picDir.exists()) {
        if(!picDir.mkpath(".")) {
            QMessageBox::warning(this, "Printscreen", "Error : Can't save file to default pictures location");
            return;
        }
    }

    auto fileName = time.toString("yyyyMMdd_hhmmsszzz") + QChar('_') + project()->currentPageName() + QString(".jpg");
    auto filePath = picDir.absoluteFilePath(fileName);

    buffer.save(filePath, "JPG", 100);
}

void BoardPage::keyboardText()
{
    QStringList pagesNames = mBoard->project()->pages().keys();

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(pagesNames.begin(),
              pagesNames.end(),
              collator);

    auto index = pagesNames.indexOf(QRegExp("^" + QRegExp::escape(mKeyboardText) + ".+"));
    if(index >= 0) {
        auto requestedPage = pagesNames.at(index);
        mBoard->project()->requestPage(requestedPage);
    }
    mKeyboardText.clear();
}

QSize BoardPage::minimumSizeHint() const
{
    return QSize(0,0);
}

void BoardPage::keyPressEvent(QKeyEvent *event)
{
    if((event->modifiers() == Qt::NoModifier ||  event->modifiers() == Qt::KeypadModifier)
            && event->count() == 1) {
        mKeyboardText.append(event->text());
        mKeyboardTimer->start();
    }

    QWidget::keyPressEvent(event);
}

Board *BoardPage::board() const
{
    return mBoard;
}

void BoardPage::setMode(BoardToolbar::DashboardMode mode)
{
    switch(mode) {
    case BoardToolbar::dmLive:
        mBoard->setLayoutLocked(true);
        ui->editionWidget->setVisible(false);
        mToolBar->loadFirstPage();
        break;
    case BoardToolbar::dmReplay:
        mBoard->setLayoutLocked(true);
        ui->editionWidget->setVisible(false);
        mToolBar->loadFirstPage();
        break;
    case BoardToolbar::dmLiveDesign:
        mBoard->setLayoutLocked(false);
        ui->editionWidget->setVisible(true);
        break;
    case BoardToolbar::dmReplayDesign:
        mBoard->setLayoutLocked(false);
        ui->editionWidget->setVisible(true);
        break;
    }

    ui->parameterPicker->updateParameterList();

    mToolBar->setMode(mode);
}

BoardToolbar *BoardPage::toolBar()
{
    return mToolBar;
}

QSharedPointer<Project> BoardPage::project() const
{
    if(mBoard)
        return mBoard->project();
    else
        return nullptr;
}

void BoardPage::savePage()
{
    mBoard->savePage();
}

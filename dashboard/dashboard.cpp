#include "dashboard/dashboard.h"

#include <utility>
#include "dashboard/layouts/layout_reactive.h"
#include "project/project.h"
#include "dashboard/elements_base/adjust_text_element.h"
#include "dashboard/dashboard_element.h"

QTBoard::QTBoard(QWidget *parent) : QCustomPlot (parent),
    mBoardColor(QColor(45,55,65)),
    mBackColor(QColor(25,35,45)),
    mFrontColor(QColor(120,130,140))
{
    setAcceptDrops(true);

    mProject = QSharedPointer<QTBProject>(new QTBProject());

    setOpenGl(false);
    setPlottingHints(QCP::phCacheLabels|QCP::phImmediateRefresh|QCP::phFastPolylines);
    plotLayout()->clear();

    initStyle();

    addLayer("elements_background", layer("background"), QCustomPlot::limAbove);

    mDashboardLayout = new QTBLayoutReactive(this);
    mDashboardLayout->initializeParentPlot(this);

    connect(this, &QCustomPlot::mouseMove,
            mDashboardLayout, &QTBLayoutReactive::mouseMove);
    connect(this, &QCustomPlot::mouseDoubleClick,
            mDashboardLayout, &QTBLayoutReactive::mouseDoubleClick);
    connect(this, &QCustomPlot::mousePress,
            mDashboardLayout, &QTBLayoutReactive::mousePress);
    connect(this, &QCustomPlot::mouseRelease,
            mDashboardLayout, &QTBLayoutReactive::mouseRelease);

    connect(mDashboardLayout, &QTBLayoutReactive::pageModified, [=](){
        mPageModified = true;
    });

    connect(mProject.data(), &QTBProject::pageRequested, this, &QTBoard::loadPage);
    connect(mProject.data(), &QTBProject::loaded, this, &QTBoard::clearPage);

    plotLayout()->addElement(mDashboardLayout);
    mDashboardLayout->setLocked(false);

    clearPage();

}

QTBoard::~QTBoard()
{
    delete mDashboardLayout;
}

QFont QTBoard::fontLight() const
{
    return mFontLight;
}

QFont QTBoard::fontRegular() const
{
    return mFontRegular;
}

QFont QTBoard::fontBlack() const
{
    return mFontBlack;
}

QFont QTBoard::fontLightItalic() const
{
    return mFontLightItalic;
}

QTBLayoutReactive *QTBoard::dashboardLayout() const
{
    return mDashboardLayout;
}

QSharedPointer<QTBDataManager> QTBoard::dataManager() const
{
    return mDataManager;
}

void QTBoard::droppedParameterSettings(QDropEvent *event)
{
    auto * element = qobject_cast<QTBDashboardElement *>(mDashboardLayout->elementAt(event->posF()));
    if(element) {
        const QMimeData *mimeData = event->mimeData();
        QByteArray encoded = mimeData->data( QTBoard::parameterSettingsMimeType() );

        QDataStream stream(&encoded, QIODevice::ReadOnly);
        QString label, descr;
        stream >> label >> descr;
        element->droppedParameter(mProject->parameterSettings(label, descr));

        mPageModified = true;

        event->accept();
    }
}

void QTBoard::droppedDataParameter(QDropEvent *event)
{
    auto * element = qobject_cast<QTBDashboardElement *>(mDashboardLayout->elementAt(event->posF()));
    if(element) {
        const QMimeData *mimeData = event->mimeData();
        QByteArray encoded = mimeData->data( QTBoard::dataParameterMimeType() );

        QDataStream stream(&encoded, QIODevice::ReadOnly);
        QString label = QString::fromStdString(encoded.toStdString());

        if(mDataManager) {
            QSharedPointer<QTBParameter> param = mDataManager->parameter(label);
            if(param) {
                element->droppedParameter(param);
                mPageModified = true;
            }
        }

        event->accept();
    }
}

void QTBoard::checkModification()
{
    if(mPageModified && mProject->currentPage()) {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard );
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Save)
        {
            savePage();
        }
    }
}

void QTBoard::update(QDateTime time)
{
    mReferenceTime = std::move(time);
    emit timeUpdate(mReferenceTime);

    if(!mLoadingPage) {
        //        QElapsedTimer timer;
        //            timer.start();

        //        mDataManager->updateData();
        replot();

        //        qDebug() << "The update operation took (milliseconds)\t" << mProject->currentPageName() << "\t" << timer.elapsed();
    }
}

QColor QTBoard::frontColor() const
{
    return mFrontColor;
}

QColor QTBoard::backColor() const
{
    return mBackColor;
}

void QTBoard::loadHistoricalData()
{
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<QTBDashboardElement*>(mDashboardLayout->elementAt(i))) {
            el->processHistoricalSamples();
        }
    }
}

void QTBoard::initDataManager()
{
    mDataManager = QSharedPointer<QTBDataManager>(new QTBDataManager());
    connect(mDataManager.data(), &QTBDataManager::parametersUpdated, this, &QTBoard::checkParameters);
    connect(mDataManager.data(), &QTBDataManager::dataUpdated, this, &QTBoard::update);
}

void QTBoard::dragEnterEvent(QDragEnterEvent *event)
{
    if(mProject->currentPage()) {
        if (event->mimeData()->hasFormat(QTBoard::elementMimeType())
                || event->mimeData()->hasFormat(QTBoard::parameterSettingsMimeType())
                || event->mimeData()->hasFormat(QTBoard::dataParameterMimeType())) {
            event->accept();
        }
    }
}

void QTBoard::dropEvent(QDropEvent *event)
{
    if(mProject->currentPage()) {
        if (event->mimeData()->hasFormat(QTBoard::elementMimeType())) {
            mDashboardLayout->droppedElement(event);
        } else if (event->mimeData()->hasFormat(QTBoard::parameterSettingsMimeType())) {
            droppedParameterSettings(event);
        } else if (event->mimeData()->hasFormat(QTBoard::dataParameterMimeType())) {
            droppedDataParameter(event);
        }
    }
}

void QTBoard::dragMoveEvent(QDragMoveEvent *event)
{
    if(mProject->currentPage()) {
        if (event->mimeData()->hasFormat(QTBoard::elementMimeType())) {
            event->accept();
            mDashboardLayout->draggedElement(event);
        } else if (event->mimeData()->hasFormat(QTBoard::parameterSettingsMimeType())
                   || event->mimeData()->hasFormat(QTBoard::dataParameterMimeType())) {
            event->accept();
        }
    }
}

void QTBoard::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void QTBoard::initStyle()
{
    int idLight = QFontDatabase::addApplicationFont(":Roboto-Light.ttf");
    QString familyLight = QFontDatabase::applicationFontFamilies(idLight).at(0);
    mFontLight = QFont(familyLight);

    mFontLightItalic = QFont(familyLight);
    mFontLightItalic.setItalic(true);

    int idRegular = QFontDatabase::addApplicationFont(":Roboto-Regular.ttf");
    QString familyRegular = QFontDatabase::applicationFontFamilies(idRegular).at(0);
    mFontRegular = QFont(familyRegular);

    int idBlack = QFontDatabase::addApplicationFont(":Roboto-Black.ttf");
    QString familyBlack = QFontDatabase::applicationFontFamilies(idBlack).at(0);
    mFontBlack = QFont(familyBlack);

    setFont(mFontLight);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, mBoardColor);
    plotGradient.setColorAt(1, mBoardColor);
    setBackground(plotGradient);

    //Blue
    QColorDialog::setStandardColor(0, QColor(QString("#1F60C4")));
    QColorDialog::setStandardColor(1, QColor(QString("#3274D9")));
    QColorDialog::setStandardColor(2, QColor(QString("#5794F2")));
    QColorDialog::setStandardColor(3, QColor(QString("#8AB8FF")));
    QColorDialog::setStandardColor(4, QColor(QString("#C0D8FF")));

    QColorDialog::setStandardColor(5, QColor(QString("#a0afb9")));

    //Purple
    QColorDialog::setStandardColor(6, QColor(QString("#8F3BB8")));
    QColorDialog::setStandardColor(7, QColor(QString("#A352CC")));
    QColorDialog::setStandardColor(8, QColor(QString("#B877D9")));
    QColorDialog::setStandardColor(9, QColor(QString("#CA95E5")));
    QColorDialog::setStandardColor(10, QColor(QString("#DEB6F2")));

    QColorDialog::setStandardColor(11, QColor(QString("#a0afb9")));

    //Green
    QColorDialog::setStandardColor(12, QColor(QString("#37872D")));
    QColorDialog::setStandardColor(13, QColor(QString("#56A64B")));
    QColorDialog::setStandardColor(14, QColor(QString("#73BF69")));
    QColorDialog::setStandardColor(15, QColor(QString("#96D98D")));
    QColorDialog::setStandardColor(16, QColor(QString("#C8F2C2")));

    QColorDialog::setStandardColor(17, QColor(QString("#a0afb9")));

    //Yellow
    QColorDialog::setStandardColor(18, QColor(QString("#E0B400")));
    QColorDialog::setStandardColor(19, QColor(QString("#F2CC0C")));
    QColorDialog::setStandardColor(20, QColor(QString("#FADE2A")));
    QColorDialog::setStandardColor(21, QColor(QString("#FFEE52")));
    QColorDialog::setStandardColor(22, QColor(QString("#FFF899")));

    QColorDialog::setStandardColor(23, QColor(QString("#a0afb9")));

    //Orange
    QColorDialog::setStandardColor(24, QColor(QString("#FA6400")));
    QColorDialog::setStandardColor(25, QColor(QString("#FF780A")));
    QColorDialog::setStandardColor(26, QColor(QString("#FF9830")));
    QColorDialog::setStandardColor(27, QColor(QString("#FFB357")));
    QColorDialog::setStandardColor(28, QColor(QString("#FFCB7D")));

    QColorDialog::setStandardColor(29, QColor(QString("#a0afb9")));

    //Red
    QColorDialog::setStandardColor(30, QColor(QString("#C4162A")));
    QColorDialog::setStandardColor(31, QColor(QString("#E02F44")));
    QColorDialog::setStandardColor(32, QColor(QString("#F2495C")));
    QColorDialog::setStandardColor(33, QColor(QString("#FF7383")));
    QColorDialog::setStandardColor(34, QColor(QString("#FFA6B0")));

    for(int i=35; i< 48;i++)
        QColorDialog::setStandardColor(i, QColor(QString("#a0afb9")));

    for(int i=0; i< 16;i++)
        QColorDialog::setCustomColor(i, QColor(QString("#a0afb9")));

}

bool QTBoard::liveDataRefreshEnabled() const
{
    return mLiveDataRefreshEnabled;
}

void QTBoard::setLiveDataRefreshEnabled(bool liveDataRefreshEnabled)
{
    if(mLiveDataRefreshEnabled != liveDataRefreshEnabled) {
        if(liveDataRefreshEnabled)
            loadHistoricalData();

        mLiveDataRefreshEnabled = liveDataRefreshEnabled;
    }
}

double QTBoard::currentTimestamp()
{
    return mReferenceTime.time().msecsSinceStartOfDay() / 1000.0;
}

void QTBoard::clearPage()
{
    mDashboardLayout->clearLayout();
    mProject->setCurrentPageName(QString(""));

    auto *text= new QTBAdjustTextElement(this);
    text->setText("Create or Open a page");
    text->setMaxPointSize(12);
    text->setTextFlags(Qt::AlignCenter);

    mDashboardLayout->addElement(text, 0, 0, mDashboardLayout->columnCount(), mDashboardLayout->rowCount());
    mDashboardLayout->setElementDraggable(0, false);
    mDashboardLayout->setElementResizable(0, false);
    mDashboardLayout->setElementEditable(0, false);
    mDashboardLayout->setElementClosable(0, false);
}

void QTBoard::loadPage()
{
    checkModification();

    QTBPage *page = mProject->requestedPage();
    if(page) {

        mLoadingPage = true;
        mDashboardLayout->clearLayout();

        mDashboardLayout->setRowCount(page->rowCount());
        mDashboardLayout->setColumnCount(page->columnCount());
        mDashboardLayout->setSingleElementRowCount(page->singleElementRowCount());
        mDashboardLayout->setSingleElementColumnCount(page->singleElementColumnCount());

        QPixmap pic(page->background());
        setBackground(pic, true, Qt::IgnoreAspectRatio);

        page->loadPageElements(this);
        mPageModified = false;
        mProject->setCurrentPageName(page->name());

        loadHistoricalData();

        mLoadingPage = false;
    }
}

void QTBoard::savePage()
{
    QTBPage *page = mProject->currentPage();
    if(page) {
        page->savePageElements(this);
        mPageModified = false;
    }
}

void QTBoard::checkParameters()
{
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<QTBDashboardElement*>(mDashboardLayout->elementAt(i))) {
            el->checkParameters();
        }
    }
}

QSharedPointer<QTBProject> QTBoard::project() const
{
    return mProject;
}


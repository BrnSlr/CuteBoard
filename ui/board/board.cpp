#include <utility>
#include <QStandardPaths>

#include "ui/board/board.h"
#include "ui/board/board_layout.h"
#include "ui/board/management/project.h"
#include "ui/board/board_element.h"
#include "ui/board/factory/element_factory_helper.h"
#include "ui/widgets/element_picker_widget.h"

bool SortElementRect(const ElementIdRect &a, const ElementIdRect &b) {
    if(std::get<2>(a) != std::get<2>(b)) {
        return (std::get<2>(a) < std::get<2>(b));
    }
    return (std::get<1>(a) < std::get<1>(b));
}

Board::Board(QWidget *parent) : QCustomPlot (parent),    
    mRefreshTimerPeriod(100),
    mLayoutLocked(true),
    mFirstReplot(true),
    mFullScreenParent(nullptr)
{
    setAcceptDrops(true);

    QApplication::setDoubleClickInterval(300);

    addLayer("elements_background", layer("background"), QCustomPlot::limBelow);
    addLayer("layout_background", layer("elements_background"), QCustomPlot::limBelow);
    addLayer("tracer", layer("overlay"), QCustomPlot::limBelow);

    layer(QLatin1String("grid"))->setMode(QCPLayer::lmBuffered);
    layer(QLatin1String("main"))->setMode(QCPLayer::lmBuffered);
    layer(QLatin1String("axes"))->setMode(QCPLayer::lmBuffered);
    layer(QLatin1String("elements_background"))->setMode(QCPLayer::lmBuffered);
    layer(QLatin1String("tracer"))->setMode(QCPLayer::lmBuffered);

    mLiveRefreshTimer = new QTimer(nullptr); // _not_ this!
    mLiveRefreshTimer->setInterval(mRefreshTimerPeriod);
    mLiveRefreshTimer->setTimerType(Qt::PreciseTimer);
    connect(mLiveRefreshTimer, SIGNAL(timeout()), this, SLOT(updateLive()), Qt::DirectConnection);

    mProject = QSharedPointer<Project>(new Project());

    setPlottingHints(QCP::phCacheLabels|QCP::phImmediateRefresh/*|QCP::phFastPolylines*/);
    plotLayout()->clear();

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    mEmptyBoardLabel = new QLabel("Create or Open a page", this);
    mEmptyBoardLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(mEmptyBoardLabel);

    mDashboardLayout = new BoardLayout(this);

    plotLayout()->addElement(mDashboardLayout);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, palette().shadow().color());
    plotGradient.setColorAt(1, palette().shadow().color());
    mDashboardLayout->setBackground(plotGradient);
    setBackground(Qt::NoBrush);

    mHoverRect = new SelectionRect(this);
    mHoverRect->setColor(QApplication::palette().linkVisited().color());
    mHoverRect->hide();

    mSaveRect = new SelectionRect(this);
    mSaveRect->setColor(QApplication::palette().text().color());
    mSaveRect->setBrushActive(false);
    mSaveRect->setPenActive(true);
    mSaveRect->setResizeHandlesActive(false);
    mSaveRect->hide();

    mResizeOrientation = SelectionRect::hhNone;

    mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    mAnimationGroup = new QParallelAnimationGroup(this);

    mContextMenu = new QMenu();

    mEditElementAction = new QAction("Edit element");
    mContextMenu->addAction(mEditElementAction);
    connect(mEditElementAction, &QAction::triggered, this, &Board::editElement);

    mClearElementsAction = new QAction("Clear element(s)");
    mClearElementsAction->setShortcut(QKeySequence(tr("Back")));
    mContextMenu->addAction(mClearElementsAction);
    connect(mClearElementsAction, &QAction::triggered, this, &Board::clearElements);

    auto clearElementsShortcut = new QShortcut(QKeySequence(QKeySequence::Back), this);
    connect(clearElementsShortcut, &QShortcut::activated,
            this, &Board::clearElements);

    mDeleteElementsAction = new QAction("Delete element(s)");
    mDeleteElementsAction->setShortcut(QKeySequence(tr("Del")));
    connect(mDeleteElementsAction, &QAction::triggered, this, &Board::deleteElements);
    mContextMenu->addAction(mDeleteElementsAction);

    auto deleteElementsShortcut = new QShortcut(QKeySequence(QKeySequence::Delete), this);
    connect(deleteElementsShortcut, &QShortcut::activated,
            this, &Board::deleteElements);

    mContextMenu->addSeparator();

    mCutElementsAction = new QAction("Cut");
    mCutElementsAction->setShortcut(QKeySequence(tr("Ctrl+X")));
    connect(mCutElementsAction, &QAction::triggered, this, &Board::cutElements);
    mContextMenu->addAction(mCutElementsAction);

    auto cutElementsShortcut = new QShortcut(QKeySequence(tr("Ctrl+X")), this);
    connect(cutElementsShortcut, &QShortcut::activated,
            this, &Board::cutElements);

    mCopyElementsAction = new QAction("Copy");
    mCopyElementsAction->setShortcut(QKeySequence(tr("Ctrl+C")));
    connect(mCopyElementsAction, &QAction::triggered, this, &Board::copyElements);
    mContextMenu->addAction(mCopyElementsAction);

    auto copyElementsShortcut = new QShortcut(QKeySequence(tr("Ctrl+C")), this);
    connect(copyElementsShortcut, &QShortcut::activated,
            this, &Board::copyElements);

    mPasteElementsAction = new QAction("Paste");
    mPasteElementsAction->setShortcut(QKeySequence(tr("Ctrl+V")));
    connect(mPasteElementsAction, &QAction::triggered, this, &Board::pasteElements);
    mContextMenu->addAction(mPasteElementsAction);

    auto pasteElementsShortcut = new QShortcut(QKeySequence(tr("Ctrl+V")), this);
    connect(pasteElementsShortcut, &QShortcut::activated,
            this, &Board::pasteElements);

    clearPage();
    mColorIndex = 1;

    connect(mDashboardLayout, &BoardLayout::pageModified, [=](){
        mPageModified = true;
    });

    connect(mProject.data(), &Project::pageRequested, this, &Board::loadPage);
    connect(mProject.data(), &Project::loaded, this, &Board::clearPage);

    mFullscreenLockedShortcut = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F11), this);
    //    connect(mFullscreenLockedShortcut, &QShortcut::activated,
    //            this, &Board::toggleFullscreen);

    mDataManager = QSharedPointer<DataManager>(new DataManager());
    connect(mDataManager.data(), &DataManager::parametersUpdated, this, &Board::checkParameters);
    //    connect(mDataManager.data(), &DataManager::reinitPlayback, this, &Board::resetPlayback);
}

Board::~Board()
{
    //    checkModification();

    mLiveRefreshTimer->stop();
    mLiveRefreshTimer->deleteLater();

    for(int i=0; i < mSelectedRects.size(); i++) {
        auto prev = mSelectedRects.takeAt(i);
        prev.clear();
    }
    mSelectedRects.clear();

    delete mSaveRect;
    delete mHoverRect;
    delete mAnimationGroup;
    delete mRubberBand;

    delete mEditElementAction;
    delete mClearElementsAction;
    delete mDeleteElementsAction;
    delete mCutElementsAction;
    delete mCopyElementsAction;
    delete mPasteElementsAction;
    delete mContextMenu;

    delete mDashboardLayout;
}

BoardLayout *Board::dashboardLayout() const
{
    return mDashboardLayout;
}

QSharedPointer<DataManager> Board::dataManager() const
{
    return mDataManager;
}

void Board::checkModification()
{
    if(mPageModified &&
            mProject &&
            mProject->currentPage()) {
        QMessageBox msgBox;
        msgBox.setText(QString("The page %1\n has been modified.").arg(mProject->currentPage()->name()));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Abort );
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Save) {
            savePage();
        }
        mPageModified = false;
    }
}

void Board::updateLive()
{
    if(mBoardReplotting)
        return;

    mBoardReplotting = true;
    QElapsedTimer timer;
    timer.start();

    if(!mPageLoading) {
        updateElements();
        if(liveDataRefreshEnabled()) {
            mDataManager->liveReferenceTime(mReferenceLiveTime);
            emit timeUpdate(mReferenceLiveTime);
            updateLiveData();
        }
        partialReplot();
        updateFinished();
    }

    auto ms = timer.elapsed();

    if(ms > mRefreshTimerPeriod) {
        logger()->warn()  << QString("Board replot too slow - %1 ms").arg(ms);
    }
    mBoardReplotting = false;
}

void Board::startLiveTimer()
{
    mDataManager->liveDataEnabled(true);
    if(mUserInteractions) {
        setInteraction(QCP::iRangeDrag, false);
        setInteraction(QCP::iRangeZoom, false);
    }
    setMouseInteractions(false);
    replot(rpQueuedReplot);
    mLiveRefreshTimer->start();
}

void Board::stopLiveTimer()
{
    mLiveRefreshTimer->stop();
    mDataManager->liveDataEnabled(false);
    setMouseInteractions(true);
    if(mUserInteractions) {
        setInteraction(QCP::iRangeDrag, true);
        setInteraction(QCP::iRangeZoom, true);
    }
    updatePlaybackData();
    replot(rpQueuedReplot);
}

void Board::replotLayer(QString layerName)
{
    if(mBoardReplotting)
        return;
    layer(layerName)->replot();
}

void Board::updateElements(bool forceUpdate)
{
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
            el->beforeDataUpdate(forceUpdate);
        }
    }
}

void Board::updateFinished()
{
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
            el->resetModifications();
        }
    }
}

void Board::partialReplot()
{
    updateLayout();
    layer(QLatin1String("grid"))->replot();
    layer(QLatin1String("main"))->replot();
    layer(QLatin1String("axes"))->replot();

    if(mMouseInteractions)
        layer(QLatin1String("tracer"))->replot();
}

void Board::updateLiveData()
{
    mDataManager->updateLiveBoardData();

    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
            el->loadData();
            el->displayData();
        }
    }
}

void Board::updatePlayback(int msecSinceStartOfTheDay, QDate date)
{    
    mReferencePlaybackTime = msecSinceStartOfTheDay;
    mReferencePlaybackDate = date;
    if(!mDataManager->liveDataEnabled()) {
        updateElements();
        updatePlaybackData();
        partialReplot();
        updateFinished();
    }
}

void Board::updatePlaybackData()
{
    if(mDataManager) {
        bool reload = mDataManager->setPlaybackDateTime(double(mReferencePlaybackTime) / 1000., mReferencePlaybackDate);
        if(reload) {
            dataManager()->clearPlaybackBoardData();
            checkParameters();
        }
        mDataManager->updatePlaybackBoardData();

        for(int i=0; i< mDashboardLayout->elementCount();i++) {
            if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
                el->loadHistoricalData();
                el->displayData();
            }
        }
    }
}

void Board::resetPlayback()
{
    dataManager()->clearPlaybackBoardData();
    checkParameters();
    updatePlaybackData();
}

void Board::editElement()
{
    if(mLayoutLocked)
        return;

    if(mSelectedIndexes.count() == 1) {
        auto *element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(mSelectedIndexes.first()));
        if(element) {
            element->edit();
            setPageModified(true);
        }
    }
}

void Board::clearElements()
{
    if(mLayoutLocked)
        return;
    for(auto index : mSelectedIndexes) {
        auto *element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(index));
        if(element) {
            element->removeBoardParameters();
        }
    }
    setPageModified(true);
    replot(QCustomPlot::rpQueuedReplot);
}

void Board::deleteElements()
{
    if(mLayoutLocked)
        return;
    QVector<BoardElement*> elements;
    for(auto index : mSelectedIndexes) {
        auto *element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(index));
        if(element) {
            elements.append(element);
        }
    }

    for(auto element : elements) {
        mDashboardLayout->take(element);
        element->clearElement();
        element->deleteLater();
    }
    setPageModified(true);

    mSelectedIndexes.clear();
    for(int i=0; i < mSelectedRects.size(); i++) {
        auto prev = mSelectedRects.takeAt(i);
        prev.clear();
    }
    mSelectedRects.clear();
    replot(QCustomPlot::rpQueuedReplot);


    auto mouseBoardPos = mapFromGlobal(QCursor::pos());
    auto layoutMargins = mDashboardLayout->margins();
    int indexCol = int((mouseBoardPos.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
    int indexRow = int((mouseBoardPos.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());

    int indexElement = mDashboardLayout->elementIndex(indexCol, indexRow);

    if(indexElement > -1) {
        auto rect = mDashboardLayout->elementAt(indexElement)->outerRect();

        auto handle = mHoverRect->hoverHandle(QCursor::pos());
        switch(handle) {
        case SelectionRect::hhNone:
            setCursor(QCursor(Qt::ArrowCursor));
            break;
        case SelectionRect::hhTopLeft:
            setCursor(QCursor(Qt::SizeFDiagCursor));
            break;
        case SelectionRect::hhTopRight:
            setCursor(QCursor(Qt::SizeBDiagCursor));
            break;
        case SelectionRect::hhBottomLeft:
            setCursor(QCursor(Qt::SizeBDiagCursor));
            break;
        case SelectionRect::hhBottomRight:
            setCursor(QCursor(Qt::SizeFDiagCursor));
            break;
        }

        if(mHoverRect->geometry() != rect.toRect()) {
            mHoverRect->hide();
            mHoverRect->setGeometry(rect.toRect());
            mHoverRect->setResizeHandlesActive(true);
            mHoverRect->update();
            mHoverRect->show();
        }
    } else {
        QRectF elementRect = QRectF(layoutMargins.left() + indexCol *  mDashboardLayout->columnWidth() + double(mDashboardLayout->columnSpacing() / 2.0),
                                    layoutMargins.top() + indexRow * mDashboardLayout->rowHeight() + double(mDashboardLayout->rowSpacing() / 2.0),
                                    mDashboardLayout->columnWidth() - mDashboardLayout->columnSpacing(),
                                    mDashboardLayout->rowHeight() - mDashboardLayout->rowSpacing());

        if(mHoverRect->geometry() != elementRect.toRect()) {
            mHoverRect->hide();
            mHoverRect->setGeometry(elementRect.toRect());
            mHoverRect->setResizeHandlesActive(false);
            mHoverRect->update();
            mHoverRect->show();
        }
    }
}

void Board::cutElements()
{
    if(mLayoutLocked)
        return;
    copyElements();
    deleteElements();
    setPageModified(true);
}

void Board::copyElements()
{
    if(mLayoutLocked)
        return;
    QVector<ElementIdRect> elementIds;
    for(auto index : mSelectedIndexes) {
        auto gridRect = mDashboardLayout->elementGridRectAt(index);
        ElementIdRect id = std::make_tuple(index, gridRect.x(), gridRect.y());
        elementIds.append(id);
    }

    std::sort(elementIds.begin(), elementIds.end(), SortElementRect);

    QTemporaryFile tmpFile;
    QString tmpFileName;
    if(tmpFile.open()) {
        tmpFileName = tmpFile.fileName() + QString(".copy_elements");
        tmpFile.close();
    }

    if(!tmpFileName.isEmpty()) {

        QSettings settings(tmpFileName, QSettings::IniFormat);
        settings.beginWriteArray("Elements");
        for(int i = 0; i< elementIds.count(); i++) {
            auto *element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(std::get<0>(elementIds.at(i))));
            if(element) {

                settings.setArrayIndex(i);
                settings.setValue("name", element->name());

                settings.beginGroup("ElementGeometry");
                mDashboardLayout->saveElementRectSettings(element, &settings);
                settings.endGroup();

                settings.beginGroup("ElementSettings");
                element->saveSettings(&settings);
                settings.endGroup();

                settings.beginGroup("ElementConfigurations");
                element->saveConfigurations(&settings);
                settings.endGroup();
            }
        }
        settings.endArray();

        settings.sync();

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(Board::elementCopyMimeType(),
                          tmpFileName.toUtf8());

        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);
    }
}

void Board::pasteElements()
{
    if(mLayoutLocked)
        return;
    if(mHoverRect->isVisible()) {
        QClipboard *clipboard = QApplication::clipboard();
        auto mimeData = clipboard->mimeData();
        QByteArray encoded = mimeData->data( Board::elementCopyMimeType() );
        QString tmpFileName = QString::fromUtf8(encoded);

        if(!tmpFileName.isEmpty()) {
            QSettings settings(tmpFileName, QSettings::IniFormat);

            int count = settings.beginReadArray("Elements");

            int indexCol = mDashboardLayout->colIndex(mHoverRect->pos().x());
            int indexRow = mDashboardLayout->rowIndex(mHoverRect->pos().y());

            int firstCol = 0;
            int firstRow = 0;

            int offsetCol = 0;
            int offsetRow = 0;

            for(int i=0; i< count;i++) {
                settings.setArrayIndex(i);

                QString elementName = settings.value("name").toString();

                settings.beginGroup("ElementGeometry");
                QRect elementRect = mDashboardLayout->loadElementRectSettings(&settings);
                settings.endGroup();

                if(i==0) {
                    firstCol = elementRect.x();
                    firstRow = elementRect.y();
                }

                offsetCol = elementRect.x() - firstCol;
                offsetRow = elementRect.y() - firstRow;

                BoardElement *element = ElementFactory::Factory()->createElement(elementName);
                if(element) {
                    element->initialize(this);

                    settings.beginGroup("ElementSettings");
                    element->loadSettings(&settings);
                    settings.endGroup();

                    if(mDashboardLayout->addElement(element,
                                                    true,
                                                    indexCol + offsetCol,
                                                    indexRow + offsetRow,
                                                    elementRect.width(),
                                                    elementRect.height())) {
                        settings.beginGroup("ElementConfigurations");
                        element->loadConfigurations(&settings);
                        settings.endGroup();

                        element->loadHistoricalData();
                        element->rebuildElement();
                    }
                }
            }
            settings.endArray();
        }
    }
    setPageModified(true);
}

void Board::dropEvent(QDropEvent *event)
{
    if(!mLayoutLocked) {
        if(mProject->currentPage()) {
            auto mouseBoardPos = event->pos();
            auto layoutMargins = mDashboardLayout->margins();
            int indexCol = int((mouseBoardPos.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
            int indexRow = int((mouseBoardPos.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());

            int indexElement = mDashboardLayout->elementIndex(indexCol, indexRow);

            if (event->mimeData()->hasFormat(Board::elementNameMimeType())) {
                if(indexElement == -1) {
                    const QMimeData *mimeData = event->mimeData();
                    QByteArray encoded = mimeData->data( Board::elementNameMimeType() );
                    QString elementName = QString::fromUtf8(encoded);

                    auto boardElement = ElementFactory::Factory()->createElement(elementName);
                    if(boardElement) {
                        addElement(boardElement, event->posF());
                        boardElement->rebuildElement();
                        for(int i=0; i < mSelectedRects.size(); i++) {
                            auto prev = mSelectedRects.takeAt(i);
                            prev.clear();
                        }
                        mSelectedRects.clear();
                        mSelectedIndexes.clear();
                    }
                }
            } else if (event->mimeData()->hasFormat(Board::propertyNameMimeType())) {

                const QMimeData *mimeData = event->mimeData();
                QByteArray encoded = mimeData->data( Board::propertyNameMimeType() );
                QDataStream stream(&encoded, QIODevice::ReadOnly);
                QString label, descr;
                stream >> label >> descr;
                auto paramSettings = mProject->parameterSettings(label, descr);
                if(paramSettings) {
                    auto element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(indexElement));
                    if(!element) {

                        QDialog dial;
                        dial.setWindowTitle("Element Picker");
                        QVBoxLayout *lay = new QVBoxLayout();
                        ElementPickerWidget *elemntPicker = new ElementPickerWidget(&dial,
                                                                                    BoardElement::etTimeSeries);

                        auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                              | QDialogButtonBox::Cancel);

                        connect(elemntPicker, &ElementPickerWidget::elementDoubleClicked, [&dial, elemntPicker](){
                            if(!elemntPicker->selectedElement().isEmpty())
                                dial.accept();
                        });
                        connect(buttonBox, &QDialogButtonBox::accepted, [&dial, elemntPicker](){
                            if(!elemntPicker->selectedElement().isEmpty())
                                dial.accept();
                        });
                        connect(buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));

                        lay->addWidget(elemntPicker);
                        lay->addWidget(buttonBox);
                        dial.setLayout(lay);
                        dial.setMinimumSize(300,350);
                        int res = dial.exec();

                        if( res == QDialog::Accepted) {
                            QString elementName = elemntPicker->selectedElement();
                            auto el = ElementFactory::Factory()->createElement(elementName);
                            if(el) {
                                addElement(el, event->posF());
                                element = qobject_cast<BoardElement *>(el);
                            }
                        }
                    }

                    if(element) {
                        element->droppedParameter(paramSettings);

                        for(int i=0; i < mSelectedRects.size(); i++) {
                            auto prev = mSelectedRects.takeAt(i);
                            prev.clear();
                        }
                        mSelectedRects.clear();
                        mSelectedIndexes.clear();

                        event->accept();
                    }
                }
            } else if (event->mimeData()->hasFormat(Board::parameterNameMimeType())) {
                const QMimeData *mimeData = event->mimeData();
                QByteArray encoded = mimeData->data( Board::parameterNameMimeType() );

                QDataStream stream(&encoded, QIODevice::ReadOnly);
                QStringList listLabels;
                stream >> listLabels;

                auto boardElement = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(indexElement));
                if(!boardElement) {

                    BoardElement::TimeSeriesType tsType = BoardElement::tstUnknown;
                    for(auto label:listLabels) {
                        QSharedPointer<TimeSeries> param = mDataManager->dataParameter(label);
                        if(param) {
                            if(tsType == BoardElement::tstUnknown) {
                                if(param->size() > 1)
                                    tsType = BoardElement::tstVector;
                                else
                                    tsType = BoardElement::tstScalar;
                            } else {
                                if(param->size() > 1) {
                                    if(tsType == BoardElement::tstScalar) {
                                        tsType = BoardElement::tstScalarAndVector;
                                        break;
                                    }
                                } else {
                                    if(tsType == BoardElement::tstVector) {
                                        tsType = BoardElement::tstScalarAndVector;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    BoardElement::TimeSeriesSize tsSize = BoardElement::tssSingle;
                    if(listLabels.count() > 1)
                        tsSize = BoardElement::tssMulti;

                    QDialog dial;
                    dial.setWindowTitle("Element Picker");
                    QVBoxLayout *lay = new QVBoxLayout();
                    ElementPickerWidget *elemntPicker = new ElementPickerWidget(&dial,
                                                                                BoardElement::etTimeSeries,
                                                                                tsType,
                                                                                tsSize);

                    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                          | QDialogButtonBox::Cancel);

                    connect(elemntPicker, &ElementPickerWidget::elementDoubleClicked, [&dial, elemntPicker](){
                        if(!elemntPicker->selectedElement().isEmpty())
                            dial.accept();
                    });
                    connect(buttonBox, &QDialogButtonBox::accepted, [&dial, elemntPicker](){
                        if(!elemntPicker->selectedElement().isEmpty())
                            dial.accept();
                    });
                    connect(buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));

                    lay->addWidget(elemntPicker);
                    lay->addWidget(buttonBox);
                    dial.setLayout(lay);
                    dial.setMinimumSize(300,350);
                    int res = dial.exec();

                    if( res == QDialog::Accepted) {
                        QString elementName = elemntPicker->selectedElement();
                        auto el = ElementFactory::Factory()->createElement(elementName);
                        if(el) {
                            addElement(el, event->posF());
                            boardElement = qobject_cast<BoardElement *>(el);
                        }
                    }
                }

                if(boardElement) {
                    if(mDataManager) {
                        for(auto label:listLabels) {
                            QSharedPointer<TimeSeries> param = mDataManager->dataParameter(label);
                            if(param) {
                                bool replace = false;
                                if(boardElement->parametersCount() > 0 && boardElement->timeSeriesSize() == BoardElement::tssSingle) {
                                    replace = true;
                                    if(event->keyboardModifiers() & Qt::ControlModifier)
                                        replace = false;
                                }
                                boardElement->droppedParameter(param, replace);
                            }
                        }

                        for(int i=0; i < mSelectedRects.size(); i++) {
                            auto prev = mSelectedRects.takeAt(i);
                            prev.clear();
                        }
                        mSelectedRects.clear();
                        mSelectedIndexes.clear();

                        replot(QCustomPlot::rpQueuedReplot);
                        event->accept();
                    }
                }
            } else if (event->mimeData()->hasFormat(Board::alarmConfigMimeType())) {
                const QMimeData *mimeData = event->mimeData();
                QString label = QString::fromStdString(mimeData->data( Board::alarmConfigMimeType() ).toStdString());
                auto alarm =  mProject->alarmConfiguration(label);

                auto element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(indexElement));
                if(alarm) {
                    if(!element) {
                        BoardElement::ElementType type = BoardElement::etAlarm;

                        QDialog dial;
                        dial.setWindowTitle("Element Picker");
                        QVBoxLayout *lay = new QVBoxLayout();
                        ElementPickerWidget *elemntPicker = new ElementPickerWidget(&dial,type);

                        auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                              | QDialogButtonBox::Cancel);

                        connect(elemntPicker, &ElementPickerWidget::elementDoubleClicked, [&dial, elemntPicker](){
                            if(!elemntPicker->selectedElement().isEmpty())
                                dial.accept();
                        });
                        connect(buttonBox, &QDialogButtonBox::accepted, [&dial, elemntPicker](){
                            if(!elemntPicker->selectedElement().isEmpty())
                                dial.accept();
                        });
                        connect(buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));

                        lay->addWidget(elemntPicker);
                        lay->addWidget(buttonBox);
                        dial.setLayout(lay);
                        dial.setMinimumSize(300,350);
                        int res = dial.exec();

                        if( res == QDialog::Accepted) {
                            QString elementName = elemntPicker->selectedElement();
                            auto el = ElementFactory::Factory()->createElement(elementName);
                            if(el) {
                                addElement(el, event->posF());
                                element = qobject_cast<BoardElement *>(el);
                            }
                        }
                    }

                    if(element) {
                        if (element->type() == BoardElement::etAlarm) {
                            element->addAlarm(alarm);

                            for(int i=0; i < mSelectedRects.size(); i++) {
                                auto prev = mSelectedRects.takeAt(i);
                                prev.clear();
                            }
                            mSelectedRects.clear();
                            mSelectedIndexes.clear();

                            event->accept();
                        } else {
                            logger()->debug() << "WRONG TYPE";
                        }
                    } else {
                        logger()->debug() << "NO ELEMENT";
                    }
                } else {
                    logger()->debug() << "NO ALARM";
                }
            }

            if(mDataManager) {
                if(mDataManager->liveDataEnabled()) {
                    loadHistoricalData();
                    replot(rpQueuedReplot);
                } else {
                    resetPlayback();
                    replot(rpQueuedReplot);
                }
            }

            mHoverRect->hide();
            setPageModified(true);
        }
    }
}

void Board::dragEnterEvent(QDragEnterEvent *event)
{
    if(!mLayoutLocked) {
        if(mProject->currentPage()) {
            if (event->mimeData()->hasFormat(Board::elementNameMimeType())
                    || event->mimeData()->hasFormat(Board::propertyNameMimeType())
                    || event->mimeData()->hasFormat(Board::alarmConfigMimeType())
                    || event->mimeData()->hasFormat(Board::parameterNameMimeType())) {
                event->accept();
            }
        }
    }
}

void Board::dragMoveEvent(QDragMoveEvent *event)
{
    bool validDrag = false;
    if(!mLayoutLocked) {
        if(mProject->currentPage()) {
            if (event->mimeData()->hasFormat(Board::elementNameMimeType())
                    || event->mimeData()->hasFormat(Board::propertyNameMimeType())
                    || event->mimeData()->hasFormat(Board::alarmConfigMimeType())
                    || event->mimeData()->hasFormat(Board::parameterNameMimeType())) {

                auto mouseBoardPos = event->pos();
                auto layoutMargins = mDashboardLayout->margins();
                int indexCol = int((mouseBoardPos.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
                int indexRow = int((mouseBoardPos.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());

                int indexElement = mDashboardLayout->elementIndex(indexCol, indexRow);

                if(indexElement > -1) {

                    if(mSelectedIndexes.contains(indexElement)) {
                        mHoverRect->hide();
                    } else {
                        auto rect = mDashboardLayout->elementAt(indexElement)->outerRect();
                        if(mHoverRect->geometry() != rect.toRect()) {
                            mHoverRect->hide();
                            mHoverRect->setGeometry(rect.toRect());
                            mHoverRect->update();
                            mHoverRect->show();
                        }
                    }
                } else {

                    QRectF elementRect = QRectF(layoutMargins.left() + indexCol *  mDashboardLayout->columnWidth() + double(mDashboardLayout->columnSpacing() / 2.0),
                                                layoutMargins.top() + indexRow * mDashboardLayout->rowHeight() + double(mDashboardLayout->rowSpacing() / 2.0),
                                                mDashboardLayout->columnWidth() - mDashboardLayout->columnSpacing(),
                                                mDashboardLayout->rowHeight() - mDashboardLayout->rowSpacing());

                    if(mHoverRect->geometry() != elementRect.toRect()) {
                        mHoverRect->hide();
                        mHoverRect->setGeometry(elementRect.toRect());
                        mHoverRect->update();
                        mHoverRect->show();
                    }
                }

                if (event->mimeData()->hasFormat(Board::elementNameMimeType())) {
                    if(indexElement == -1)
                        validDrag = true;
                } else if (event->mimeData()->hasFormat(Board::propertyNameMimeType())) {
                    if(indexElement > -1) {
                        auto boardElement = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(indexElement));
                        if(boardElement && boardElement->type() == BoardElement::etTimeSeries) {
                            validDrag = true;
                        }
                    } else {
                        validDrag = true;
                    }
                } else if (event->mimeData()->hasFormat(Board::alarmConfigMimeType())) {
                    if(indexElement > -1) {
                        auto boardElement = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(indexElement));
                        if(boardElement && boardElement->type() == BoardElement::etAlarm) {
                            validDrag = true;
                        }
                    } else {
                        validDrag = true;
                    }
                } else if (event->mimeData()->hasFormat(Board::parameterNameMimeType())) {
                    const QMimeData *mimeData = event->mimeData();
                    QByteArray encoded = mimeData->data( Board::parameterNameMimeType() );

                    QDataStream stream(&encoded, QIODevice::ReadOnly);
                    QStringList listLabels;
                    stream >> listLabels;


                    if(indexElement > -1) {
                        auto boardElement = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(indexElement));
                        if(boardElement && boardElement->type() == BoardElement::etTimeSeries) {

                            BoardElement::TimeSeriesType tsType = BoardElement::tstUnknown;
                            for(auto label:listLabels) {
                                QSharedPointer<TimeSeries> param = mDataManager->dataParameter(label);
                                if(param) {
                                    if(tsType == BoardElement::tstUnknown) {
                                        if(param->size() > 1)
                                            tsType = BoardElement::tstVector;
                                        else
                                            tsType = BoardElement::tstScalar;
                                    } else {
                                        if(param->size() > 1) {
                                            if(tsType == BoardElement::tstScalar) {
                                                tsType = BoardElement::tstScalarAndVector;
                                                break;
                                            }
                                        } else {
                                            if(tsType == BoardElement::tstVector) {
                                                tsType = BoardElement::tstScalarAndVector;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            if(boardElement->timeSeriesType() == tsType)
                                validDrag = true;
                        }
                    } else {
                        validDrag = true;
                    }
                }
            }
        }
    }
    if(validDrag)
        event->accept();
    else
        event->ignore();
}

void Board::dragLeaveEvent(QDragLeaveEvent *event)
{
    mHoverRect->hide();
    event->accept();
}

void Board::mousePressEvent(QMouseEvent *event)
{
    if(mLayoutLocked) {
        if((event->modifiers() & Qt::ControlModifier) && mMouseInteractions) {
            setSelectionRectMode(QCP::srmZoom);
        } else {
            setSelectionRectMode(QCP::srmNone);
        }
    }

    mMousePos = event->pos();

    QCustomPlot::mousePressEvent(event);

    if(!mLayoutLocked) {
        if(mAnimationGroup->state() == QAnimationGroup::Running)
            return;
        auto mouseBoardPos = mapFromGlobal(event->globalPos());
        auto layoutMargins = mDashboardLayout->margins();
        int indexCol = int((mouseBoardPos.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
        int indexRow = int((mouseBoardPos.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());

        auto index = mDashboardLayout->elementIndex(indexCol, indexRow);

        if(index > -1) {
            mHoverRect->hide();

            if ( event->modifiers() & Qt::ControlModifier) {
                if(mSelectedIndexes.contains(index)) {
                    auto id = mSelectedIndexes.indexOf(index);
                    mSelectedIndexes.removeAt(id);
                    auto prev = mSelectedRects.takeAt(id);
                    prev.clear();

                    bool resizeActive = true;
                    if(mSelectedRects.count() > 1) {
                        resizeActive = false;
                    }

                    for(auto elRect : mSelectedRects) {
                        elRect->setResizeHandlesActive(resizeActive);
                        elRect->update();
                    }

                    mHoverRect->show();
                    mResizeOrientation = SelectionRect::hhNone;
                } else {

                    auto selectionRect = QSharedPointer<SelectionRect>(new SelectionRect(this), &QObject::deleteLater);
                    selectionRect->setBrushActive(true);
                    selectionRect->setResizeHandlesActive(true);
                    selectionRect->hide();

                    mSelectedRects.append(selectionRect);
                    mSelectedIndexes.append(index);

                    bool resizeActive = true;
                    if(mSelectedRects.count() > 1) {
                        resizeActive = false;
                    }

                    for(auto elRect : mSelectedRects) {
                        elRect->setResizeHandlesActive(resizeActive);
                        elRect->update();
                    }

                    auto rect = mDashboardLayout->elementAt(index)->outerRect();
                    if(selectionRect->geometry() != rect.toRect()) {
                        selectionRect->hide();
                        selectionRect->setGeometry(rect.toRect());
                        selectionRect->update();
                        selectionRect->raise();
                        selectionRect->show();
                    }

                    mResizeOrientation = SelectionRect::hhNone;
                }
            } else {

                if(!mSelectedIndexes.contains(index)) {

                    for(int i=0; i < mSelectedRects.size(); i++) {
                        auto prev = mSelectedRects.takeAt(i);
                        prev.clear();
                    }
                    mSelectedRects.clear();
                    mSelectedIndexes.clear();

                    auto selectionRect = QSharedPointer<SelectionRect>(new SelectionRect(this), &QObject::deleteLater);
                    selectionRect->setBrushActive(true);
                    selectionRect->setResizeHandlesActive(true);
                    selectionRect->hide();

                    mSelectedRects.append(selectionRect);
                    mSelectedIndexes.append(index);

                    auto rect = mDashboardLayout->elementAt(index)->outerRect();
                    if(selectionRect->geometry() != rect.toRect()) {
                        selectionRect->hide();
                        selectionRect->setGeometry(rect.toRect());
                        selectionRect->update();
                        selectionRect->raise();
                        selectionRect->show();
                    }

                    mResizeOrientation = selectionRect->hoverHandle(event->globalPos());
                } else {
                    mResizeOrientation = mSelectedRects.first()->hoverHandle(event->globalPos());
                }
            }
        } else {
            for(int i=0; i < mSelectedRects.size(); i++) {
                auto prev = mSelectedRects.takeAt(i);
                prev.clear();
            }
            mSelectedRects.clear();
            mSelectedIndexes.clear();

            if (event->button() == Qt::LeftButton) {
                mRubberBandOrigin = event->pos();
                mRubberBand->setGeometry(
                            QRect(mRubberBandOrigin, QSize()));
                mRubberBand->raise();
                mRubberBand->show();
            }
        }

        if (event->button() == Qt::RightButton) {

            QClipboard *clipboard = QApplication::clipboard();
            auto mimeData = clipboard->mimeData();
            QByteArray encoded = mimeData->data( Board::elementCopyMimeType() );
            QString tmpFileName = QString::fromUtf8(encoded);

            mPasteElementsAction->setEnabled(false);
            if(mSelectedIndexes.count() == 0) {
                mEditElementAction->setEnabled(false);
                mClearElementsAction->setEnabled(false);
                mDeleteElementsAction->setEnabled(false);
                mCutElementsAction->setEnabled(false);
                mCopyElementsAction->setEnabled(false);
                if(!tmpFileName.isEmpty()) {
                    mPasteElementsAction->setEnabled(true);
                }
            } else {
                if(mSelectedIndexes.count() == 1)
                    mEditElementAction->setEnabled(true);
                else
                    mEditElementAction->setEnabled(false);
                mClearElementsAction->setEnabled(true);
                mDeleteElementsAction->setEnabled(true);
                mCutElementsAction->setEnabled(true);
                mCopyElementsAction->setEnabled(true);
            }

            mContextMenu->exec(event->globalPos() + QPoint(3,3));
        }
    }
}

void Board::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    if(!mLayoutLocked) {

        if(mAnimationGroup->state() == QAnimationGroup::Running)
            return;

        if(mMouseHasMoved) {
            if (event->buttons() & Qt::LeftButton) {
                mHoverRect->hide();
                if(mRubberBand->isVisible()) {
                    auto rect = QRect(mRubberBandOrigin, event->pos()).normalized();
                    mRubberBand->setGeometry(rect);

                    auto layoutMargins = mDashboardLayout->margins();

                    int col = qRound((rect.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
                    int row = qRound((rect.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());
                    int width = qRound(rect.width() / mDashboardLayout->columnWidth());
                    int height = qRound(rect.height() / mDashboardLayout->rowHeight());

                    if(width < 1) {
                        width = 1;
                    }

                    if(height < 1){
                        height = 1;
                    }
                    for(int i=0; i < mSelectedRects.size(); i++) {
                        auto prev = mSelectedRects.takeAt(i);
                        prev.clear();
                    }
                    mSelectedRects.clear();
                    mSelectedIndexes.clear();

                    for(int i=0;i<width;i++) {
                        for(int j=0;j<height;j++) {
                            auto index = mDashboardLayout->elementIndex(col + i, row + j);
                            if(index >= 0 && !mSelectedIndexes.contains(index)) {


                                auto selectionRect = QSharedPointer<SelectionRect>(new SelectionRect(this), &QObject::deleteLater);
                                selectionRect->setBrushActive(true);
                                selectionRect->setResizeHandlesActive(false);
                                selectionRect->hide();

                                mSelectedRects.append(selectionRect);
                                mSelectedIndexes.append(index);

                                auto rect = mDashboardLayout->elementAt(index)->outerRect();
                                if(selectionRect->geometry() != rect.toRect()) {
                                    selectionRect->hide();
                                    selectionRect->setGeometry(rect.toRect());
                                    selectionRect->update();
                                    selectionRect->raise();
                                    selectionRect->show();
                                }
                            }
                        }
                    }

                } else {
                    if(mSelectedIndexes.size() > 0) {

                        QVector<bool> validRects;
                        QVector<bool> validPositions;
                        QVector<bool> lockedElements;
                        QVector<int> newCols;
                        QVector<int> newRows;
                        QVector<int> newWidths;
                        QVector<int> newHeights;

                        for(int k = 0; k<mSelectedIndexes.count(); k++) {
                            int indexSelection = mSelectedIndexes.at(k);
                            auto rect = mSelectedRects.at(k)->geometry();

                            bool validRect = false;
                            switch(mResizeOrientation) {
                            case SelectionRect::hhNone :
                            {
                                setCursor(QCursor(Qt::SizeAllCursor));
                                int xShift = event->pos().x() - mMousePos.x();
                                int yShift = event->pos().y() - mMousePos.y();

                                rect.translate(xShift, yShift);
                                validRect = true;
                                break;
                            }
                            case SelectionRect::hhTopLeft :
                                setCursor(QCursor(Qt::SizeFDiagCursor));
                                rect.setTopLeft(event->pos());
                                if(rect.x() >= 0 && rect.y() >= 0 &&
                                        rect.width() > 0 && rect.height() > 0)
                                    validRect = true;
                                break;
                            case SelectionRect::hhTopRight :
                                setCursor(QCursor(Qt::SizeBDiagCursor));
                                rect.setTopRight(event->pos());
                                if(rect.x() >= 0 && rect.y() >= 0 &&
                                        rect.width() > 0 && rect.height() > 0)
                                    validRect = true;
                                break;
                            case SelectionRect::hhBottomRight :
                                setCursor(QCursor(Qt::SizeFDiagCursor));
                                rect.setBottomRight(event->pos());
                                if(rect.x() >= 0 && rect.y() >= 0 &&
                                        rect.width() > 0 && rect.height() > 0)
                                    validRect = true;
                                break;
                            case SelectionRect::hhBottomLeft :
                                setCursor(QCursor(Qt::SizeBDiagCursor));
                                rect.setBottomLeft(event->pos());
                                if(rect.x() >= 0 && rect.y() >= 0 &&
                                        rect.width() > 0 && rect.height() > 0)
                                    validRect = true;
                                break;
                            }

                            bool validPosition = true;
                            bool locked = false;
                            int draggedElementNewColumn = -1;
                            int draggedElementNewRow = -1;
                            int newDragWidth = 0;
                            int newDragHeight = 0;

                            if(validRect) {
                                mSelectedRects.at(k)->setGeometry(rect);
                                mSelectedRects.at(k)->update();

                                auto layoutMargins = mDashboardLayout->margins();

                                draggedElementNewColumn = qRound((rect.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
                                draggedElementNewRow = qRound((rect.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());
                                newDragWidth = qRound(rect.width() / mDashboardLayout->columnWidth());
                                newDragHeight = qRound(rect.height() / mDashboardLayout->rowHeight());

                                auto gridRect = mDashboardLayout->elementGridRectAt(indexSelection);

                                if(newDragWidth < 1) {
                                    locked = true;
                                    newDragWidth = 1;
                                }

                                if(newDragHeight < 1){
                                    locked = true;
                                    newDragHeight = 1;
                                }

                                if(draggedElementNewColumn < 0){
                                    locked = true;
                                    draggedElementNewColumn = 0;
                                }

                                if(draggedElementNewRow < 0){
                                    locked = true;
                                    draggedElementNewRow = 0;
                                }

                                if(draggedElementNewColumn + newDragWidth >  mDashboardLayout->columnCount()){
                                    locked = true;
                                    draggedElementNewColumn = mDashboardLayout->columnCount() - gridRect.width();
                                    newDragWidth = gridRect.width();
                                }

                                if(draggedElementNewRow + newDragHeight > mDashboardLayout->rowCount()){
                                    locked = true;
                                    draggedElementNewRow = mDashboardLayout->rowCount() - gridRect.height();
                                    newDragHeight =  gridRect.height();
                                }

                                newRows.append(draggedElementNewRow);
                                newCols.append(draggedElementNewColumn);
                                newWidths.append(newDragWidth);
                                newHeights.append(newDragHeight);

                                for(int i=0;i<newDragWidth;i++) {
                                    for(int j=0;j<newDragHeight;j++) {
                                        auto index = mDashboardLayout->elementIndex(draggedElementNewColumn + i, draggedElementNewRow + j);
                                        if(index >= 0 && !mSelectedIndexes.contains(index)) {
                                            validPosition = false;
                                            break;
                                        }
                                    }
                                }
                            }
                            validRects.append(validRect);
                            validPositions.append(validPosition);
                            lockedElements.append(locked);
                        }

                        bool canMove = true;
                        for(auto st : validRects)
                            canMove &= st;
                        for(auto st : validPositions)
                            canMove &= st;
                        for(auto st : lockedElements)
                            canMove &= !st;

                        if(canMove) {
                            for(int k = 0; k<mSelectedIndexes.count(); k++) {
                                mDashboardLayout->setElementGeometry(mSelectedIndexes.at(k),
                                                                     newCols.at(k),
                                                                     newRows.at(k),
                                                                     newWidths.at(k),
                                                                     newHeights.at(k));
                            }
                            replot(QCustomPlot::rpQueuedReplot);
                        }
                    }
                }
            } else {
                auto mouseBoardPos = mapFromGlobal(event->globalPos());
                auto layoutMargins = mDashboardLayout->margins();
                int indexCol = int((mouseBoardPos.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
                int indexRow = int((mouseBoardPos.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());

                int indexElement = mDashboardLayout->elementIndex(indexCol, indexRow);

                if(indexElement > -1) {

                    if(mSelectedIndexes.contains(indexElement)) {
                        mHoverRect->hide();
                        if(mSelectedIndexes.count() > 1) {
                            setCursor(QCursor(Qt::ArrowCursor));
                        } else {
                            auto handle = mSelectedRects.at(mSelectedIndexes.indexOf(indexElement))->hoverHandle(event->globalPos());
                            switch(handle) {
                            case SelectionRect::hhNone:
                                setCursor(QCursor(Qt::ArrowCursor));
                                break;
                            case SelectionRect::hhTopLeft:
                                setCursor(QCursor(Qt::SizeFDiagCursor));
                                break;
                            case SelectionRect::hhTopRight:
                                setCursor(QCursor(Qt::SizeBDiagCursor));
                                break;
                            case SelectionRect::hhBottomLeft:
                                setCursor(QCursor(Qt::SizeBDiagCursor));
                                break;
                            case SelectionRect::hhBottomRight:
                                setCursor(QCursor(Qt::SizeFDiagCursor));
                                break;
                            }
                        }
                    } else {
                        auto rect = mDashboardLayout->elementAt(indexElement)->outerRect();

                        auto handle = mHoverRect->hoverHandle(event->globalPos());
                        switch(handle) {
                        case SelectionRect::hhNone:
                            setCursor(QCursor(Qt::ArrowCursor));
                            break;
                        case SelectionRect::hhTopLeft:
                            setCursor(QCursor(Qt::SizeFDiagCursor));
                            break;
                        case SelectionRect::hhTopRight:
                            setCursor(QCursor(Qt::SizeBDiagCursor));
                            break;
                        case SelectionRect::hhBottomLeft:
                            setCursor(QCursor(Qt::SizeBDiagCursor));
                            break;
                        case SelectionRect::hhBottomRight:
                            setCursor(QCursor(Qt::SizeFDiagCursor));
                            break;
                        }

                        if(mHoverRect->geometry() != rect.toRect()) {
                            mHoverRect->hide();
                            mHoverRect->setGeometry(rect.toRect());
                            mHoverRect->setResizeHandlesActive(true);
                            mHoverRect->update();
                            mHoverRect->show();
                        }
                    }
                } else {

                    QRectF elementRect = QRectF(layoutMargins.left() + indexCol *  mDashboardLayout->columnWidth() + double(mDashboardLayout->columnSpacing() / 2.0),
                                                layoutMargins.top() + indexRow * mDashboardLayout->rowHeight() + double(mDashboardLayout->rowSpacing() / 2.0),
                                                mDashboardLayout->columnWidth() - mDashboardLayout->columnSpacing(),
                                                mDashboardLayout->rowHeight() - mDashboardLayout->rowSpacing());

                    if(mHoverRect->geometry() != elementRect.toRect()) {
                        mHoverRect->hide();
                        mHoverRect->setGeometry(elementRect.toRect());
                        mHoverRect->setResizeHandlesActive(false);
                        mHoverRect->update();
                        mHoverRect->show();
                    }
                }
            }
        }
    }

    mMousePos = event->pos();
}

void Board::mouseReleaseEvent(QMouseEvent *event)
{
    QCustomPlot::mouseReleaseEvent(event);
    if(mRubberBand->isVisible()) {
        mRubberBand->hide();
    } else {

        if(mSelectedIndexes.count() > 0) {

            mAnimationGroup->clear();
            for(int i = 0; i<mSelectedIndexes.count(); i++) {
                QPropertyAnimation *animation = new QPropertyAnimation(this);
                auto selRect = mSelectedRects.at(i);
                animation->setTargetObject(selRect.data()); //text
                animation->setPropertyName("geometry");

                if (mMouseHasMoved)
                    animation->setDuration(100);
                else
                    animation->setDuration(10);
                animation->setStartValue(selRect->geometry());

                auto finalRect = mDashboardLayout->elementAt(mSelectedIndexes.at(i))->outerRect().toRect();
                finalRect.moveTopLeft((mDashboardLayout->elementAt(mSelectedIndexes.at(i))->outerRect().topLeft().toPoint()));
                animation->setEndValue(finalRect);

                connect(animation, &QPropertyAnimation::valueChanged, [selRect](){
                    selRect->update();
                });

                mAnimationGroup->addAnimation(animation);
            }
            connect(mAnimationGroup, &QAnimationGroup::finished, [=](){
                mAnimationGroup->clear();
            });

            mAnimationGroup->start(QAbstractAnimation::KeepWhenStopped);
            setCursor(QCursor(Qt::ArrowCursor));
            mResizeOrientation = SelectionRect::hhNone;
        }
    }
}

void Board::mouseDoubleClickEvent(QMouseEvent *event)
{
    QCustomPlot::mouseDoubleClickEvent(event);

    if(!mLayoutLocked) {
        if (event->button() == Qt::LeftButton) {
            auto mouseGlobalPos = QCursor::pos();
            auto mouseBoardPos = mapFromGlobal(mouseGlobalPos);

            auto layoutMargins = mDashboardLayout->margins();
            int indexCol = int((mouseBoardPos.x() - layoutMargins.left()) / mDashboardLayout->columnWidth());
            int indexRow = int((mouseBoardPos.y() - layoutMargins.top()) / mDashboardLayout->rowHeight());

            auto index = mDashboardLayout->elementIndex(indexCol, indexRow);

            if (index > -1) {
                event->accept();

                for(int i=0; i < mSelectedRects.size(); i++) {
                    auto prev = mSelectedRects.takeAt(i);
                    prev.clear();
                }
                mSelectedRects.clear();
                mSelectedIndexes.clear();

                auto selectionRect = QSharedPointer<SelectionRect>(new SelectionRect(this), &QObject::deleteLater);
                selectionRect->setBrushActive(true);
                selectionRect->setResizeHandlesActive(true);
                selectionRect->hide();

                mSelectedRects.append(selectionRect);
                mSelectedIndexes.append(index);

                auto rect = mDashboardLayout->elementAt(index)->outerRect();
                if(selectionRect->geometry() != rect.toRect()) {
                    selectionRect->hide();
                    selectionRect->setGeometry(rect.toRect());
                    selectionRect->update();
                    selectionRect->raise();
                    selectionRect->show();
                }
                auto *element = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(index));
                if(element) {
                    element->edit();
                    setPageModified(true);
                }
            } else {
                QDialog dial;
                dial.setWindowTitle("Element Picker");
                QVBoxLayout *lay = new QVBoxLayout();
                ElementPickerWidget *elemntPicker = new ElementPickerWidget(&dial,
                                                                            BoardElement::etUnknown,
                                                                            BoardElement::tstUnknown,
                                                                            BoardElement::tssUnknown);

                auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                      | QDialogButtonBox::Cancel);

                connect(elemntPicker, &ElementPickerWidget::elementDoubleClicked, [&dial, elemntPicker](){
                    if(!elemntPicker->selectedElement().isEmpty())
                        dial.accept();
                });
                connect(buttonBox, &QDialogButtonBox::accepted, [&dial, elemntPicker](){
                    if(!elemntPicker->selectedElement().isEmpty())
                        dial.accept();
                });
                connect(buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));

                lay->addWidget(elemntPicker);
                lay->addWidget(buttonBox);
                dial.setLayout(lay);
                dial.setMinimumSize(300,350);
                int res = dial.exec();

                if( res == QDialog::Accepted) {
                    QString elementName = elemntPicker->selectedElement();
                    auto el = ElementFactory::Factory()->createElement(elementName);
                    if(el) {
                        addElement(el, event->pos());
                        el->rebuildElement();
                        setPageModified(true);
                    }
                }
            }
        } else {
            if(mouseInteractions()) {
                auto mouseGlobalPos = QCursor::pos();
                auto mouseBoardPos = mapFromGlobal(mouseGlobalPos);
                if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(mouseBoardPos))) {
                    event->accept();
                    el->loadData();
                }
            }
        }
    }
}

void Board::leaveEvent(QEvent *event)
{
    if(!mLayoutLocked) {
        if(QApplication::activeModalWidget() == nullptr && QApplication::activePopupWidget() == nullptr)
            mHoverRect->hide();
        event->accept();
    }
}

QColor Board::randomColor(int index)
{
    double currentHueAngle = 137.50776;
    if(index > 0) {
        currentHueAngle = 137.50776 * index;
    } else {
        currentHueAngle = 137.50776 * mColorIndex;
        mColorIndex++;
    }
    int h = int( fmod( currentHueAngle, 360.0 ) );
    int s = 252;
    int v = 252;
    return QColor::fromHsv( h, s, v );
}

void Board::loadHistoricalData()
{
    if(mDataManager) {
        mDataManager->loadLiveBoardData();

        for(int i=0; i< mDashboardLayout->elementCount();i++) {
            if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
                el->loadHistoricalData();
                el->displayData();
            }
        }
    }
}

void Board::initDataManager()
{
    QString appPath = qApp->applicationDirPath();
    QString libPath = appPath + QDir::separator() + QString("DataSources");

    mDataManager->loadDataSources();

    mLiveRefreshTimer->start();
}

void Board::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    setViewport(rect());
    replot(rpQueuedRefresh);

    for(int i=0; i<mSelectedIndexes.count(); i++) {
        auto rect = mDashboardLayout->elementAt(mSelectedIndexes.at(i))->outerRect();
        mSelectedRects.at(i)->hide();
        mSelectedRects.at(i)->setGeometry(rect.toRect());
        mSelectedRects.at(i)->setResizeHandlesActive(true);
        mSelectedRects.at(i)->update();
        mSelectedRects.at(i)->show();
    }
}

void Board::closeEvent(QCloseEvent *event)
{
    emit widgetClosed();
    event->accept();
}

void Board::buildPalette()
{
    mColorIndex = 1;

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

bool Board::layoutLocked() const
{
    return mLayoutLocked;
}

void Board::setLayoutLocked(bool layoutLocked)
{
    if(layoutLocked != mLayoutLocked) {

        mLayoutLocked = layoutLocked;
        if(mLayoutLocked) {
            for(int i=0; i < mSelectedRects.size(); i++) {
                auto prev = mSelectedRects.takeAt(i);
                prev.clear();
            }
            mSelectedRects.clear();
            mSelectedIndexes.clear();
        }
    }
}

void Board::reloadProject()
{
    mProject->fullReload();
    loadPage();
}

bool Board::mouseInteractions() const
{
    return mMouseInteractions;
}

void Board::setRefreshTimerPeriod(int refreshTimerPeriod)
{
    mRefreshTimerPeriod = refreshTimerPeriod;
    mLiveRefreshTimer->setInterval(refreshTimerPeriod);
}

void Board::setReferencePlaybackDate(const QDate &referencePlaybackDate)
{
    mReferencePlaybackDate = referencePlaybackDate;
}

bool Board::pageModified() const
{
    return mPageModified;
}

void Board::setPageModified(bool pageModified)
{
    mPageModified = pageModified;
}

void Board::setReferencePlaybackTime(int referencePlaybackTime)
{
    mReferencePlaybackTime = referencePlaybackTime;
}

void Board::setLastPlaybackTime()
{
    bool ok;
    int lastTime = mDataManager->playbackEndTime(ok);
    if(ok)
        mReferencePlaybackTime = lastTime;
}

bool Board::statMode() const
{
    return mStatMode;
}

void Board::setUserInteractions(bool userInteractions)
{
    setInteraction(QCP::iRangeDrag, userInteractions);
    setInteraction(QCP::iRangeZoom, userInteractions);
}

void Board::setMouseInteractions(bool mouseInteractions)
{
    mMouseInteractions = mouseInteractions;
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
            el->setMouseInteractionsEnabled(mouseInteractions);
        }
    }
}

void Board::setStatMode(bool statMode)
{
    mStatMode = statMode;
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
            el->setStatModeEnabled(statMode);
        }
    }
}

bool Board::liveDataRefreshEnabled() const
{
    return mLiveDataRefreshEnabled;
}

void Board::setLiveDataRefreshEnabled(bool liveDataRefreshEnabled)
{
    if(mLiveDataRefreshEnabled != liveDataRefreshEnabled) {
        if(liveDataRefreshEnabled) {
            loadHistoricalData();

            setMouseInteractions(false);
            setUserInteractions(false);

        } else {
            setMouseInteractions(true);
            setUserInteractions(true);
        }

        for(int i=0; i< mDashboardLayout->elementCount();i++) {
            if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
                el->setBoardLiveState(liveDataRefreshEnabled);
            }
        }

        checkParameters();
        mLiveDataRefreshEnabled = liveDataRefreshEnabled;
    }
}

double Board::currentTimestamp()
{
    if(mDataManager->liveDataEnabled())
        return mReferenceLiveTime.toMSecsSinceEpoch() / 1000.0;
    else
        return mReferencePlaybackTime / 1000.0;
}

void Board::clearPage()
{
    mDashboardLayout->clearLayout();
    mProject->setCurrentPageName(QString(""));
    mEmptyBoardLabel->setVisible(true);
}

void Board::loadPage()
{
    checkModification();

    auto page = mProject->requestedPage();
    if(page) {
        logger()->info() << QString("Loading page %1").arg(page->name());

        for(int i=0; i < mSelectedRects.size(); i++) {
            auto prev = mSelectedRects.takeAt(i);
            prev.clear();
        }
        mSelectedRects.clear();
        mSelectedIndexes.clear();

        mEmptyBoardLabel->setVisible(false);
        mPageLoading = true;
        mDashboardLayout->clearLayout();

        mDashboardLayout->setRowCount(page->rowCount());
        mDashboardLayout->setColumnCount(page->columnCount());
        mDashboardLayout->setSingleElementRowCount(page->singleElementRowCount());
        mDashboardLayout->setSingleElementColumnCount(page->singleElementColumnCount());

        QPixmap pic(mProject->resourcesImagesPath() + QDir::separator() + page->background());
        mDashboardLayout->setBackground(pic, true, Qt::IgnoreAspectRatio);

        page->loadPageElements(this);
        mPageModified = false;
        mProject->setCurrentPageName(page->name());
        mPageLoading = false;

        setMouseInteractions(mMouseInteractions);
        setStatMode(mStatMode);

        if(mDataManager) {
            if(mDataManager->liveDataEnabled()) {
                loadHistoricalData();
                replot(rpQueuedReplot);
            } else {
                resetPlayback();
                replot(rpQueuedReplot);
            }
        }
    }
}

void Board::savePage()
{
    bool showSave = isVisible();

    auto page = mProject->currentPage();
    if(page) {

        auto currentCursor = cursor();
        setCursor(QCursor(Qt::WaitCursor));

        page->savePageElements(this);
        mPageModified = false;

        if(showSave) {
            mSaveRect->setGeometry(rect());
            mSaveRect->raise();
            mSaveRect->update();
            mSaveRect->show();

            QTimer::singleShot(100, mSaveRect, &SelectionRect::hide);
        }
        setCursor(currentCursor);
    }
}

void Board::checkParameters()
{
    for(int i=0; i< mDashboardLayout->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(mDashboardLayout->elementAt(i))) {
            el->checkParameters();
        }
    }
}

QSharedPointer<Project> Board::project() const
{
    return mProject;
}

bool Board::toggleFullscreen()
{
    if (!parent()) {
        if (isFullScreen())
            showNormal();
        else
            showFullScreen();
        return true;
    }

    if (!mFullScreenParent) {
        QWidget* wparent = qobject_cast<QWidget*>(parent());
        if (!wparent) {
            return false;
        }

        if (!wparent->layout()) {
            if (qobject_cast<QSplitter*>(wparent)) {
                mFullScreenParentLayoutIndex = qobject_cast<QSplitter*>(wparent)->indexOf(this);
            } else {
                return false;
            }
        } else if (qobject_cast<QBoxLayout*>(wparent->layout())) {
            mFullScreenParentLayoutIndex = wparent->layout()->indexOf(this);
        } else if (qobject_cast<QGridLayout*>(wparent->layout())) {
            QGridLayout* lay = qobject_cast<QGridLayout*>(wparent->layout());
            for (int i=0; i<lay->count (); i++) {
                mFullScreenParentGridRow = i/lay->columnCount();
                mFullScreenParentGridCol = i % lay->columnCount();
                if (lay->itemAtPosition (mFullScreenParentGridRow, mFullScreenParentGridCol)->widget()==this) break;
            }
        } else if (qobject_cast<QStackedLayout*>(wparent->layout())) {
            return false;
        } else {
            return false;
        }

        mFullScreenEmbeddedParent = wparent;

        mFullScreenParent = new QWidget();
        mFullScreenParent->setContentsMargins(0,0,0,0);
        mFullScreenParent->setLayout(new QVBoxLayout());
        mFullScreenParent->layout()->addWidget(this);
        mFullScreenParent->layout()->setContentsMargins(1,1,1,1);
        //        mFullScreenParent->windowHandle()->setScreen(windowHandle()->screen());
        mFullScreenParent->showFullScreen();

    } else {
        setParent(nullptr);

        if (qobject_cast<QSplitter*>(mFullScreenEmbeddedParent)) {
            qobject_cast<QSplitter*>(mFullScreenEmbeddedParent)->insertWidget(mFullScreenParentLayoutIndex, this);
        } else if (qobject_cast<QBoxLayout*>(mFullScreenEmbeddedParent->layout())) {
            qobject_cast<QBoxLayout*>(mFullScreenEmbeddedParent->layout())->insertWidget(mFullScreenParentLayoutIndex, this);
        } else if (qobject_cast<QGridLayout*>(mFullScreenEmbeddedParent->layout())) {
            QGridLayout* gl = qobject_cast<QGridLayout*>(mFullScreenEmbeddedParent->layout());
            gl->addWidget(this, mFullScreenParentGridRow, mFullScreenParentGridCol);
        } else if (qobject_cast<QStackedLayout*>(mFullScreenEmbeddedParent->layout())) {
        } else {
        }
        mFullScreenParent->hide();
        mFullScreenParent->deleteLater();
        mFullScreenParent = nullptr;
        mFullScreenParentLayoutIndex = 0;
    }

    return true;
}

BoardElement *Board::element(QPointF pos)
{
    return qobject_cast<BoardElement *>(mDashboardLayout->elementAt(pos));
}

void Board::addElement(BoardElement *el, QPointF pos)
{
    int indexCol = mDashboardLayout->colIndex(pos.x());
    int indexRow = mDashboardLayout->rowIndex(pos.y());
    el->initialize(this);
    mDashboardLayout->addElement(el, true, indexCol, indexRow);
}

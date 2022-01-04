#include "plot_time.h"
#include "../NumericalDisplay/numerical_display.h"
#include "plot_time_editor.h"
#include <math.h>

PlotTime::PlotTime(Board *dashboard) :
    BoardElement (dashboard),
    mLegendPosition(lpRight),
    mLegendVisible(true),
    mYAxisTicksVisible(false),
    mYAxisTicksCount(5),
    mYAxisLineVisible(false),
    mYAxisLabelsVisible(true),
    mYAxisGridVisible(true),
    mXAxisTicksVisible(false),
    mXAxisTicksCount(3),
    mXAxisLineVisible(false),
    mXAxisLabelsVisible(true),
    mXAxisGridVisible(true),
    mXAxisFixedTicker(true),
    mBoardReferenceTimeActive(true),
    mYAxisScale(asAutoZero),
    mXAxisHistory(30),
    mXAxisDirection(adRightToLeft),
    mThresholdsVisible(true),
    mTickStepMinValue(0.01),
    mTickStepMinValueEnabled(true),
    mTitleVisible(true),
    mTitle("Graphs"),
    mTracer(nullptr),
    mStatMode(smNone)
{
    setConfigurationMode(ParameterConfiguration::cmGraph);
    setParametersMaxCount(16);
}

void PlotTime::clearElement()
{
    if(mLayout) {
        for(int i=0;i<mLegendLayout->elementCount();i++) {
            auto * element = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(i));
            if(element)
                element->clearElement();
        }

        if(mTracer) {
            delete mTracer;
            mTracer = nullptr;
        }

        QList<QCPAbstractItem*> items = mAxisRect->items();
        for(int i=0; i<items.count();i++)
            mBoard->removeItem(items.at(i));

        mAxisRect->clearPlottables();
        mLegendLayout->clear();
        mLayout->clear();
        mMainLayout->clear();
        delete mMainLayout;
    }
}

void PlotTime::initialize(Board *dashboard)
{
    if(dashboard) {
        BoardElement::initialize(dashboard);

        mMainLayout = new LayoutGrid();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setLayer(QLatin1String("main"));

        mLayout = new LayoutGrid();
        mLayout->initializeParentPlot(dashboard);
        mLayout->setLayer(QLatin1String("main"));

        mTitleLabel = new AdaptiveTextElement(dashboard);
        mTitleLabel->setTextFlags(Qt::AlignCenter);
        mTitleLabel->setMaxPointSize(10);
        mTitleLabel->setMinPointSize(6);
        mTitleLabel->setAdjustStrategy(AdaptiveTextElement::asAdjustAndElide);

        QFont f = mTitleLabel->font();
        f.setUnderline(false);
        mTitleLabel->setFont(f);
        auto textColor = QApplication::palette().toolTipText().color();
        mTitleLabel->setTextColor(textColor);
        mTitleLabel->setLayer(QLatin1String("main"));
        mTitleLabel->setCachedPixmap(true);
        mTitleLabel->setText(mTitle);

        mAxisRect = new AxisRect(dashboard);
        mAxisRect->setAxisColor(textColor);
        mAxisRect->setMinimumMargins(QMargins(22,10,50,10));

        mYRightAxis = mAxisRect->axis(QCPAxis::atRight);

        mXBottomMovingAxis = mAxisRect->axis(QCPAxis::atBottom);
        mXBottomMovingAxis->setVisible(!mXAxisFixedTicker);

        mXBottomStaticAxis = mAxisRect->addAxis(QCPAxis::atBottom);
        mXBottomStaticAxis->setBasePen(Qt::NoPen);
        mXBottomStaticAxis->setTickPen(Qt::NoPen);
        mXBottomStaticAxis->setSubTickPen(Qt::NoPen);
        mXBottomStaticAxis->setTickLabelColor(textColor);
        mXBottomStaticAxis->grid()->setPen(QPen(textColor, 0, Qt::DotLine));
        mXBottomStaticAxis->grid()->setZeroLinePen(Qt::NoPen);
        mXBottomStaticAxis->grid()->setVisible(mXAxisGridVisible);
        mXBottomStaticAxis->setVisible(mXAxisFixedTicker);

        QList<QCPAxis*> zoomableAxis;
        zoomableAxis.append( mYRightAxis);
        zoomableAxis.append( mXBottomStaticAxis);
        zoomableAxis.append( mXBottomMovingAxis);

        mAxisRect->setRangeZoomAxes(zoomableAxis);
        mAxisRect->setRangeDragAxes(zoomableAxis);

        mYRightAxis->setTickLabelSide(QCPAxis::lsOutside);
        mXBottomMovingAxis->setTickLabelSide(QCPAxis::lsOutside);
        mXBottomStaticAxis->setTickLabelSide(QCPAxis::lsOutside);

        mLegendLayout = new LayoutGrid();
        mLegendLayout->initializeParentPlot(dashboard);
        mLegendLayout->setRowSpacing(0);
        mLegendLayout->setMargins(QMargins(0,0,0,0));
        mLegendLayout->setLayer(QLatin1String("main"));

        mLegendEmptyElementFirst = new QCPLayoutElement(dashboard);
        mLegendEmptyElementFirst->setMinimumSize(0.1,0.1);
        mLegendEmptyElementLast = new QCPLayoutElement(dashboard);
        mLegendEmptyElementLast->setMinimumSize(0.1,0.1);
        mLegendLayout->addElement(mLegendEmptyElementFirst);
        mLegendLayout->addElement(mLegendEmptyElementLast);
        mLegendLayout->setRowStretchFactor(0,0.01);

        mTimeTicker = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
        mTimeTicker->setDateTimeFormat("HH:mm:ss");
        mTimeTicker->setDateTimeSpec(Qt::UTC);
        mTimeTicker->setTickCount(mXAxisTicksCount);
        mTimeTicker->setTickStepStrategy(QCPAxisTicker::tssReadability);
        mXBottomMovingAxis->setTicker(mTimeTicker);

        connect(dashboard, &Board::mouseMove,
                this, &PlotTime::mouseMoveSlot);
    }
}

void PlotTime::edit()
{
    PlotTimeEditor editor(this);
    editor.exec();
}

QList<QCPLayoutElement *> PlotTime::elements(bool recursive) const
{
    Q_UNUSED(recursive)
    QList<QCPLayoutElement *> list;
    if(mAxisRect)
        list.append(mAxisRect);
    return list;
}

void PlotTime::loadSettings(QSettings *settings)
{
    settings->beginGroup("General");
    if(settings->contains("LegendVisible"))
        setLegendVisible(settings->value("LegendVisible").toBool());
    if(settings->contains("LegendPosition"))
        setLegendPosition(PlotTime::LegendPosition(settings->value("LegendPosition").toInt()));
    if(settings->contains("BackgroundMode")) {
        auto maxVal = QMetaEnum::fromType<BackgroundMode>().keyCount();
        auto val = settings->value("BackgroundMode").toInt();
        if(val <   maxVal)
            setBackgroundMode(BackgroundMode(val));
    }
    if(settings->contains("StatMode"))
        mStatMode = StatMode(settings->value("StatMode").toInt());
    mTitleVisible = settings->value("TitleVisible", false).toBool();
    mTitle = settings->value("Title", "Graphs").toString();
    settings->endGroup();

    settings->beginGroup("ValueAxis");
    if(settings->contains("TicksVisible"))
        setYAxisTicksVisible(settings->value("TicksVisible").toBool());
    if(settings->contains("TicksCount"))
        setYAxisTicksCount(settings->value("TicksCount").toInt());
    if(settings->contains("LabelsVisible"))
        setYAxisLabelsVisible(settings->value("LabelsVisible").toBool());
    if(settings->contains("LineVisible"))
        setYAxisLineVisible(settings->value("LineVisible").toBool());
    if(settings->contains("GridVisible"))
        setYAxisGridVisible(settings->value("GridVisible").toBool());
    if(settings->contains("ScaleMode"))
        setYAxisScale(YAxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setYAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setYAxisMaxCustom(settings->value("ScaleMax").toDouble());
    if(settings->contains("ThresholdsVisible"))
        setThresholdsVisible(settings->value("ThresholdsVisible").toBool());
    if(settings->contains("TickStepMinValue"))
        setTickStepMinValue(settings->value("TickStepMinValue").toDouble());
    if(settings->contains("TickStepMinValueEnabled"))
        setTickStepMinValueEnabled(settings->value("TickStepMinValueEnabled").toBool());
    settings->endGroup();

    settings->beginGroup("TimeAxis");
    if(settings->contains("TicksVisible"))
        setXAxisTicksVisible(settings->value("TicksVisible").toBool());
    if(settings->contains("TicksCount"))
        setXAxisTicksCount(settings->value("TicksCount").toInt());
    if(settings->contains("LineVisible"))
        setXAxisLineVisible(settings->value("LineVisible").toBool());
    if(settings->contains("LabelsVisible"))
        setXAxisLabelsVisible(settings->value("LabelsVisible").toBool());
    if(settings->contains("GridVisible"))
        setXAxisGridVisible(settings->value("GridVisible").toBool());
    if(settings->contains("History"))
        setXAxisHistory(settings->value("History").toDouble());
    if(settings->contains("Direction"))
        setXAxisDirection(XAxisDirection(settings->value("Direction").toInt()));
    if(settings->contains("FixedTicker"))
        setXAxisFixedTicker(settings->value("FixedTicker").toBool());
    if(settings->contains("LocalClock"))
        setBoardReferenceTimeActive(settings->value("LocalClock").toBool());
    settings->endGroup();
}

void PlotTime::loadConfigurations(QSettings *settings)
{
    BoardElement::loadConfigurations(settings);

    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<BoardParameter> dashParam = boardParameter(i);
        if(dashParam) {
            auto *legend = new NumericalDisplay(mBoard);
            legend->initialize(mBoard);
            legend->addParameter(dashParam);
            legend->setBackgroundMode(backgroundMode());
            legend->setMainElement(false);
            if(mLegendPosition == lpLeft ||
                    mLegendPosition == lpRight) {
                legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
            } else {
                legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
            }

            mLegendLayout->take(mLegendEmptyElementLast);
            mLegendLayout->simplify();
            mLegendLayout->addElement(legend);
            mLegendLayout->addElement(mLegendEmptyElementLast);
            mLegendLayout->setRowStretchFactor(mLegendLayout->rowCount()-1 , 0.01);

            new PlotTimeGraph(mXBottomMovingAxis, mYRightAxis);
        }
    }

    rebuildElement();
}

void PlotTime::saveSettings(QSettings *settings)
{
    settings->beginGroup("General");
    settings->setValue("LegendVisible", mLegendVisible);
    settings->setValue("LegendPosition", mLegendPosition);
    settings->setValue("BackgroundMode", mBackgroundMode);
    settings->setValue("StatMode", mStatMode);
    settings->setValue("TitleVisible", mTitleVisible);
    settings->setValue("Title", mTitle);
    settings->endGroup();

    settings->beginGroup("ValueAxis");
    settings->setValue("TicksVisible", mYAxisTicksVisible);
    settings->setValue("TicksCount", mYAxisTicksCount);
    settings->setValue("LineVisible", mYAxisLineVisible);
    settings->setValue("LabelsVisible", mYAxisLabelsVisible);
    settings->setValue("GridVisible", mYAxisGridVisible);
    settings->setValue("ScaleMode", mYAxisScale);
    settings->setValue("ScaleMin", mYAxisMinCustom);
    settings->setValue("ScaleMax", mYAxisMaxCustom);
    settings->setValue("ThresholdsVisible", mThresholdsVisible);
    settings->setValue("TickStepMinValue", mTickStepMinValue);
    settings->setValue("TickStepMinValueEnabled", mTickStepMinValueEnabled);
    settings->endGroup();

    settings->beginGroup("TimeAxis");
    settings->setValue("TicksVisible", mXAxisTicksVisible);
    settings->setValue("TicksCount", mXAxisTicksCount);
    settings->setValue("LineVisible", mXAxisLineVisible);
    settings->setValue("LabelsVisible", mXAxisLabelsVisible);
    settings->setValue("GridVisible", mXAxisGridVisible);
    settings->setValue("History", mXAxisHistory);
    settings->setValue("Direction", mXAxisDirection);
    settings->setValue("FixedTicker", mXAxisFixedTicker);
    settings->setValue("LocalClock", mBoardReferenceTimeActive);
    settings->endGroup();
}

void PlotTime::mouseMoveSlot(QMouseEvent *event)
{
    if(mMouseInteractionsEnabled) {
        if(mAxisRect->rect().contains(event->pos())) {
            if(mTracer) {
                mTracer->setVisible(true);
                mTracer->setMousePos(event->localPos()); //mXBottomMovingAxis->pixelToCoord(event->localPos().x()));
                mBoard->replotLayer(QLatin1String("tracer"));
            }
        } else {
            if(mTracer && mTracer->visible()) {
                mTracer->setVisible(false);
                mBoard->replotLayer(QLatin1String("tracer"));
            }
        }
    }
}

QString PlotTime::title() const
{
    return mTitle;
}

void PlotTime::setTitle(const QString &title)
{
    mTitle = title;
}

bool PlotTime::titleVisible() const
{
    return mTitleVisible;
}

void PlotTime::setTitleVisible(bool titleVisible)
{
    mTitleVisible = titleVisible;
}

bool PlotTime::tickStepMinValueEnabled() const
{
    return mTickStepMinValueEnabled;
}

void PlotTime::setTickStepMinValueEnabled(bool tickStepMinValueEnabled)
{
    mTickStepMinValueEnabled = tickStepMinValueEnabled;
}

double PlotTime::tickStepMinValue() const
{
    return mTickStepMinValue;
}

void PlotTime::setTickStepMinValue(double tickStepMinValue)
{
    mTickStepMinValue = tickStepMinValue;
    if(mTickStepMinValue <= 0)
        mTickStepMinValue = 0.0001;
}

int PlotTime::xAxisTicksCount() const
{
    return mXAxisTicksCount;
}

void PlotTime::setXAxisTicksCount(int xAxisTicksCount)
{
    mXAxisTicksCount = xAxisTicksCount;
    if(mXAxisTicksCount < 1)
        mXAxisTicksCount = 1;
}

int PlotTime::yAxisTicksCount() const
{
    return mYAxisTicksCount;
}

void PlotTime::setYAxisTicksCount(int yAxisTicksCount)
{
    mYAxisTicksCount = yAxisTicksCount;
    if(mYAxisTicksCount < 1)
        mYAxisTicksCount = 1;
}

QSharedPointer<BoardParameter> PlotTime::addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    QSharedPointer<BoardParameter> dashParam = BoardElement::addParameter(parameterSettings);
    return dashParam;
}

QSharedPointer<BoardParameter> PlotTime::addParameter(QString paramLabel)
{
    QSharedPointer<BoardParameter> dashParam = BoardElement::addParameter(paramLabel);
    return dashParam;
}

void PlotTime::addParameter(QSharedPointer<BoardParameter> dashParameter)
{
    BoardElement::addParameter(dashParameter);

    if(mLegendLayout->elementCount() == parametersMaxCount() + 2) {

        auto *legend = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(parametersCount()));
        if(legend) {
            legend->addParameter(dashParameter);
            mAxisRect->graphs().last()->data()->clear();

            if(mBoard->dataManager()) {

                QVector<DataValue> values;
                QVector<double> timestamps;

                if(mBoard->dataManager()->lastBoardData(dashParameter->getTimeSeries().parameterId(), values, timestamps)) {
                    QVector<DataValue>::const_iterator itValues;
                    QVector<double>::const_iterator itTimestamps;

                    for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                         (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                         ++itValues, ++itTimestamps) {
                        mAxisRect->graphs().last()->addData(*itTimestamps, itValues->toDouble());
                    }
                }
            }
        }
    } else {
        auto *legend = new NumericalDisplay(mBoard);
        legend->initialize(mBoard);
        legend->addParameter(dashParameter);
        legend->setBackgroundMode(backgroundMode());
        legend->setMainElement(false);
        if(mLegendPosition == lpLeft ||
                mLegendPosition == lpRight) {
            legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
        } else {
            legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
        }

        mLegendLayout->take(mLegendEmptyElementLast);
        mLegendLayout->simplify();
        mLegendLayout->addElement(legend);
        mLegendLayout->addElement(mLegendEmptyElementLast);
        mLegendLayout->setRowStretchFactor(mLegendLayout->rowCount()-1 , 0.01);

        auto graph = new PlotTimeGraph(mXBottomMovingAxis, mYRightAxis);

        if(mBoard->dataManager()) {

            QVector<DataValue> values;
            QVector<double> timestamps;
            if(mBoard->dataManager()->lastBoardData(dashParameter->getTimeSeries().parameterId(), values, timestamps)) {
                QVector<DataValue>::const_iterator itValues;
                QVector<double>::const_iterator itTimestamps;

                for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                     (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                     ++itValues, ++itTimestamps) {
                    graph->addData(*itTimestamps, itValues->toDouble());
                }
            }
        }
    }
}

QSharedPointer<BoardParameter> PlotTime::replaceParameter(int index, QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    QSharedPointer<BoardParameter> dashParam = BoardElement::replaceParameter(index,parameterSettings);
    return dashParam;
}

QSharedPointer<BoardParameter> PlotTime::replaceParameter(int index, QString paramLabel)
{
    QSharedPointer<BoardParameter> dashParam = BoardElement::replaceParameter(index,paramLabel);
    return dashParam;
}

void PlotTime::replaceParameter(int index, QSharedPointer<BoardParameter> boardParameter)
{
    if (index < mBoardParameters.count() && index >= 0) {
        BoardElement::replaceParameter(index, boardParameter);

        auto *legend = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(index + 1));
        if(legend) {
            legend->replaceParameter(0,boardParameter);
            mAxisRect->graphs().at(index)->data()->clear();

            if(mBoard->dataManager()) {

                QVector<DataValue> values;
                QVector<double> timestamps;

                if(mBoard->dataManager()->lastBoardData(boardParameter->getTimeSeries().parameterId(), values, timestamps)) {
                    QVector<DataValue>::const_iterator itValues;
                    QVector<double>::const_iterator itTimestamps;

                    for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                         (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                         ++itValues, ++itTimestamps) {
                        mAxisRect->graphs().at(index)->addData(*itTimestamps, itValues->toDouble());
                    }
                }
            }
        }
    }
}

void PlotTime::removeBoardParameter(int index, bool update)
{
    mBoard->removeGraph(mAxisRect->graphs().at(index));
    QCPLayoutElement *el = mLegendLayout->takeAt(index + 1);

    if(auto element = qobject_cast<NumericalDisplay *>(el))
        element->clearElement();

    el->deleteLater();

    mLegendLayout->simplify();
    BoardElement::removeBoardParameter(index, update);

    rebuildElement();
}

void PlotTime::checkParameters()
{
    for(int i=1; i< mLegendLayout->elementCount() - 1;i++)
        if(auto element = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(i)))
            element->checkParameters();

    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<BoardParameter> dashParam = boardParameter(i);
        if(i < mAxisRect->graphs().count()) {
            if(dashParam) {
                if(dashParam->getTimeSeries().parameterId() == 0) {
                    mAxisRect->graphs().at(i)->data()->clear();
                }
            }
        }
    }

    resetGraphsStyle();
    resetItems();
}

void PlotTime::loadData()
{
    if(mBoard->dataManager()) {
        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<BoardParameter> dashParam = boardParameter(i);
            if(i < mAxisRect->graphs().count()) {
                if(dashParam) {
                    if(dashParam->getTimeSeries().parameterId() > 0) {

                        QVector<DataValue> values;
                        QVector<double> timestamps;
                        if(mBoard->dataManager()->lastBoardData(dashParam->getTimeSeries().parameterId(), values, timestamps)) {

                            QVector<DataValue>::const_iterator itValues;
                            QVector<double>::const_iterator itTimestamps;

                            for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                                 (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                                 ++itValues, ++itTimestamps) {
                                mAxisRect->graphs().at(i)->addData(*itTimestamps, itValues->toDouble());
                            }
                        }
                    } else {
                        mAxisRect->graphs().at(i)->addData(mXBottomMovingAxis->range().upper,
                                                           qQNaN());
                    }
                } else {
                    mAxisRect->graphs().at(i)->addData(mXBottomMovingAxis->range().upper,
                                                       qQNaN());
                }
            }
        }

        for(int i=1; i< mLegendLayout->elementCount() - 1;i++)
            if(auto element = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(i)))
                element->loadData();
    }
}

void PlotTime::displayData()
{
    for(int i=1; i< mLegendLayout->elementCount() - 1;i++)
        if(auto element = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(i)))
            element->displayData();

    updateGraphsStyle();
    updateAxes();
    updateItems();
}

void PlotTime::loadHistoricalData()
{
    if(mBoard->dataManager()) {

        for(int i=1; i< mLegendLayout->elementCount() - 1;i++)
            if(auto element = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(i)))
                element->loadHistoricalData();

        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<BoardParameter> dashParam = boardParameter(i);
            if(dashParam) {
                if(i < mAxisRect->graphs().count()) {

                    QVector<DataValue> values;
                    QVector<double> timestamps;
                    if(mBoard->dataManager()->lastBoardData(dashParam->getTimeSeries().parameterId(),
                                                            values,
                                                            timestamps)) {

                        mAxisRect->graphs().at(i)->data()->clear();

                        QVector<DataValue>::const_iterator itValues;
                        QVector<double>::const_iterator itTimestamps;

                        for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                             (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                             ++itValues, ++itTimestamps) {

                            mAxisRect->graphs().at(i)->addData(*itTimestamps, itValues->toDouble());
                        }
                    }
                }
            }
        }
    }
}

void PlotTime::applySizeConstraints()
{
    QSizeF defaultSize = mBoard->dashboardLayout()->singleElementSize();

    if(mLegendVisible) {
        if(mLegendPosition == lpLeft ||
                mLegendPosition == lpRight) {
            mLegendLayout->setMinimumSize(defaultSize.width(), 0.1);
            for(int i=1; i< mLegendLayout->elementCount() - 1;i++) {
                mLegendLayout->elementAt(i)->setMinimumSize(defaultSize.width(), 0);
                mLegendLayout->elementAt(i)->setMaximumSize(defaultSize.width(), defaultSize.height());
            }
        } else {
            mLegendLayout->setMinimumSize(0.1, defaultSize.height());
            for(int i=1; i< mLegendLayout->elementCount() - 1;i++) {
                mLegendLayout->elementAt(i)->setMinimumSize(defaultSize.width(), 0);
                mLegendLayout->elementAt(i)->setMaximumSize(defaultSize.width(), defaultSize.height());
            }
        }
    }

    if(mTitleVisible) {
        defaultSize.rwidth() -= mMargins.left() + mMargins.right() + mMainLayout->margins().left() + mMainLayout->margins().right();
        defaultSize.rheight() -= mMargins.top() + mMargins.bottom() + mMainLayout->margins().top() + mMainLayout->margins().bottom();

        QSizeF currentSize = mRect.size();
        currentSize.rwidth() -= mMainLayout->margins().left() + mMainLayout->margins().right();
        currentSize.rheight() -= mMainLayout->margins().top() + mMainLayout->margins().bottom();

        QSizeF labelSize = defaultSize;

        if(currentSize.height() < defaultSize.height())
            labelSize = currentSize;

        mTitleLabel->setMinimumSize(labelSize.width(), 0.3 * labelSize.height());
    }
}

void PlotTime::updateAxes()
{
    auto refTime = mBoard->currentTimestamp();
    mXBottomStaticAxis->setRange(-mXAxisHistory,0);

    if(mBoardReferenceTimeActive) {
        mXBottomMovingAxis->setRange(mBoard->currentTimestamp() - mXAxisHistory, mBoard->currentTimestamp());
    } else {
        mXBottomMovingAxis->rescale();
        mXBottomMovingAxis->setRange(mXBottomMovingAxis->range().upper - mXAxisHistory, mXBottomMovingAxis->range().upper);
        refTime = mXBottomMovingAxis->range().upper;
    }

    for(int i=0; i< parametersCount(); i++) {
        if(i < mAxisRect->graphs().count()) {
            mAxisRect->graphs().at(i)->data()->removeBefore(refTime-mXAxisHistory);
        }
    }

    switch(mYAxisScale) {
    case asAuto:
    {

        mYRightAxis->rescale();

        if(mTickStepMinValueEnabled) {
            if(mYAxisTicksCount < 1)
                mYAxisTicksCount = 1;
            if(mYRightAxis->range().size() / mYAxisTicksCount < mTickStepMinValue) {
                mYRightAxis->scaleRange(mTickStepMinValue * mYAxisTicksCount / mYRightAxis->range().size());
            }
        }
        mAutoRange = mYRightAxis->range();

        mYAxisMinCustom = mYRightAxis->range().lower;
        mYAxisMaxCustom = mYRightAxis->range().upper;
    }
        break;
    case asAutoZero:
    {
        mYRightAxis->rescale();

        if(mTickStepMinValueEnabled) {
            if(mYAxisTicksCount < 1)
                mYAxisTicksCount = 1;
            if(mYRightAxis->range().size() / mYAxisTicksCount < mTickStepMinValue) {
                mYRightAxis->scaleRange(mTickStepMinValue * mYAxisTicksCount / mYRightAxis->range().size());
            }
        }

        if(mYRightAxis->range().upper < 0)
            mYRightAxis->setRangeUpper(0);

        if(mYRightAxis->range().lower > 0)
            mYRightAxis->setRangeLower(0);

        mAutoRange = mYRightAxis->range();

        mYAxisMinCustom = mYRightAxis->range().lower;
        mYAxisMaxCustom = mYRightAxis->range().upper;
    }
        break;
    case asParam:
    {
        bool validRange = false;
        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<BoardParameter> dashParam = boardParameter(i);
            if(dashParam) {
                if(i==0) {
                    if(dashParam->parameterConfiguration()->validRange()) {
                        validRange = true;
                        mYAxisMinCustom = dashParam->parameterConfiguration()->rangeMinimum();
                        mYAxisMaxCustom = dashParam->parameterConfiguration()->rangeMaximum();
                    }
                } else {
                    if(dashParam->parameterConfiguration()->validRange()) {
                        validRange = true;
                        if(dashParam->parameterConfiguration()->rangeMinimum() < mYAxisMinCustom)
                            mYAxisMinCustom = dashParam->parameterConfiguration()->rangeMinimum();
                        if(dashParam->parameterConfiguration()->rangeMaximum() > mYAxisMaxCustom)
                            mYAxisMaxCustom = dashParam->parameterConfiguration()->rangeMaximum();
                    }
                }
            }
        }

        if(!validRange) {
            mYRightAxis->rescale();

            if(mTickStepMinValueEnabled) {
                if(mYAxisTicksCount < 1)
                    mYAxisTicksCount = 1;
                if(mYRightAxis->range().size() / mYAxisTicksCount < mTickStepMinValue) {
                    mYRightAxis->scaleRange(mTickStepMinValue * mYAxisTicksCount / mYRightAxis->range().size());
                }
            }

            mAutoRange = mYRightAxis->range();
            mYAxisMinCustom = mYRightAxis->range().lower;
            mYAxisMaxCustom = mYRightAxis->range().upper;
        } else {
            mYRightAxis->setRange(QCPRange(mYAxisMinCustom, mYAxisMaxCustom));
        }
        break;
    }
    case asCustom:
        mYRightAxis->setRange(QCPRange(mYAxisMinCustom, mYAxisMaxCustom));
        break;
    }
}

void PlotTime::resetItems()
{
    if(mTracer) {
        delete mTracer;
        mTracer = nullptr;
    }

    if(mAxisRect) {
        QList<QCPAbstractItem*> items = mAxisRect->items();
        for(int i=0; i<items.count();i++)
            mBoard->removeItem(items.at(i));

        mStatMaxTracers.clear();
        mStatMaxTexts.clear();
        mStatMaxArrows.clear();

        mStatMinTracers.clear();
        mStatMinTexts.clear();
        mStatMinArrows.clear();


        auto textColor = QApplication::palette().toolTipText().color();
        mTracer = new PlotTimeTracer(mAxisRect);
        mTracer->setTextColor(textColor);
        mTracer->setLineColor(textColor);
        switch(mBackgroundMode) {
        case bmBackground:
            mTracer->setTextBrush(QBrush(QApplication::palette().mid().color()));
            break;
        case bmTransparent:
            mTracer->setTextBrush(QBrush(QApplication::palette().shadow().color()));
            break;
        }
        mTracer->initialize();
        mTracer->setYAxisType(QCPAxis::atRight);

        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<BoardParameter> dashParam = boardParameter(i);
            if(dashParam) {
                PlotTimeGraph *graph = nullptr;
                if(i < mAxisRect->graphs().count()) {
                    graph = qobject_cast<PlotTimeGraph*>(mAxisRect->graphs().at(i));
                    if(graph)
                        graph->clearColoredSegments();
                }
                double lower = -std::numeric_limits<double>::max();
                QMap<double, ColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();

                QMap<double, ColorSettings>::iterator lowIt;
                for (lowIt = lowThr.begin();
                     lowIt != lowThr.end(); ++lowIt) {

                    if(mThresholdsVisible && dashParam->parameterConfiguration()->itemsThresholdsVisible()) {
                        auto* l = new QCPItemStraightLine(mBoard);
                        l->setLayer(QLatin1String("grid"));
                        l->point1->setType(QCPItemPosition::ptPlotCoords);
                        l->point2->setType(QCPItemPosition::ptPlotCoords);
                        l->point1->setAxes(mXBottomMovingAxis, mYRightAxis);
                        l->point2->setAxes(mXBottomMovingAxis, mYRightAxis);
                        l->point1->setAxisRect(mAxisRect);
                        l->point2->setAxisRect(mAxisRect);
                        l->point1->setCoords(0.0, lowIt.key());
                        l->point2->setCoords(1.0, lowIt.key());
                        l->setClipAxisRect(mAxisRect);
                        l->setClipToAxisRect(true);
                        QColor color = lowIt.value().color();
                        color.setAlpha(150);
                        QPen pen = QPen(color, 0, Qt::DashDotDotLine);
                        l->setPen(pen);
                    }

                    if(graph && dashParam->parameterConfiguration()->itemColorMode() == ParameterConfiguration::icDynamicSegmented) {

                        QBrush brush;
                        switch(dashParam->parameterConfiguration()->graphBrush()) {
                        case ParameterConfiguration::bsNone :
                        {
                            brush = Qt::NoBrush;
                        }
                            break;
                        case ParameterConfiguration::bsFilled :
                        {
                            QColor color = lowIt.value().color();
                            color.setAlpha(50);
                            brush = QBrush(color);
                        }
                            break;
                        case ParameterConfiguration::bsGradient :
                            QColor color = lowIt.value().color();
                            color.setAlpha(200);

                            QLinearGradient gradient(0, 1, 0, 0);
                            gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
                            gradient.setColorAt(1, color);
                            gradient.setColorAt(0.1, QColor::fromRgbF(0, 0, 0, 0));
                            gradient.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0));
                            brush = QBrush(gradient);
                            break;
                        }

                        graph->addColoredSegment(lower, lowIt.key(), QPen(lowIt.value().color(), dashParam->parameterConfiguration()->penWidth()), brush);
                        lower = lowIt.key();
                    }
                }

                QMap<double, ColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();

                QMapIterator<double, ColorSettings> highIt(highThr);
                highIt.toBack();
                double upper = std::numeric_limits<double>::max();
                while (highIt.hasPrevious()) {
                    highIt.previous();
                    if(mThresholdsVisible && dashParam->parameterConfiguration()->itemsThresholdsVisible()) {
                        auto* l = new QCPItemStraightLine(mBoard);
                        l->setLayer(QLatin1String("grid"));
                        l->point1->setType(QCPItemPosition::ptPlotCoords);
                        l->point2->setType(QCPItemPosition::ptPlotCoords);
                        l->point1->setAxes(mXBottomMovingAxis, mYRightAxis);
                        l->point2->setAxes(mXBottomMovingAxis, mYRightAxis);
                        l->point1->setAxisRect(mAxisRect);
                        l->point2->setAxisRect(mAxisRect);
                        l->point1->setCoords(0.0, highIt.key());
                        l->point2->setCoords(1.0, highIt.key());
                        l->setClipAxisRect(mAxisRect);
                        l->setClipToAxisRect(true);
                        QColor color = highIt.value().color();
                        color.setAlpha(150);
                        QPen pen = QPen(color, 0, Qt::DashDotDotLine);
                        l->setPen(pen);
                    }

                    if(graph && dashParam->parameterConfiguration()->itemColorMode() == ParameterConfiguration::icDynamicSegmented) {
                        QBrush brush;
                        switch(dashParam->parameterConfiguration()->graphBrush()) {
                        case ParameterConfiguration::bsNone :
                        {
                            brush = Qt::NoBrush;
                        }
                            break;
                        case ParameterConfiguration::bsFilled :
                        {
                            QColor color = highIt.value().color();
                            color.setAlpha(50);
                            brush = QBrush(color);
                        }
                            break;
                        case ParameterConfiguration::bsGradient :
                            QColor color = highIt.value().color();
                            color.setAlpha(200);

                            QLinearGradient gradient(0, 1, 0, 0);
                            gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
                            gradient.setColorAt(1, color);
                            gradient.setColorAt(0.1, QColor::fromRgbF(0, 0, 0, 0));
                            gradient.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0));
                            brush = QBrush(gradient);
                            break;
                        }

                        graph->addColoredSegment(highIt.key(), upper, QPen(highIt.value().color(),dashParam->parameterConfiguration()->penWidth()), brush);
                        upper = highIt.key();
                    }
                }
            }
        }

        ColorSettings colorSet;
        colorSet.setMode(ColorSettings::cmFilledBackground);

        bool maxVisible = true;
        if(!mStatModeEnabled)
            maxVisible = false;
        if(mStatMode == smNone || mStatMode == smMin)
            maxVisible = false;

        bool minVisible = true;
        if(!mStatModeEnabled)
            minVisible = false;
        if(mStatMode == smNone || mStatMode == smMax)
            minVisible = false;

        for(auto graph : mAxisRect->graphs()) {
            auto maxTracer = new QCPItemTracer(mBoard);
            maxTracer->position->setType(QCPItemPosition::ptPlotCoords);
            maxTracer->position->setAxisRect(mAxisRect);
            maxTracer->position->setAxes(mXBottomMovingAxis, mYRightAxis);
            maxTracer->setGraph(graph);
            maxTracer->setClipAxisRect(mAxisRect);
            maxTracer->setClipToAxisRect(true);
            maxTracer->setStyle(QCPItemTracer::tsCircle);
            maxTracer->setPen(graph->pen());
            maxTracer->setBrush(graph->pen().color());
            maxTracer->setSize(7);
            maxTracer->setVisible(maxVisible);
            maxTracer->setInterpolating(false);

            mStatMaxTracers.append(maxTracer);

            auto maxArrow = new QCPItemLine(mBoard);
            maxArrow->end->setParentAnchor(maxTracer->position);
            maxArrow->start->setParentAnchor(maxArrow->end);
            maxArrow->start->setCoords(15, 15);
            maxArrow->start->setAxisRect(mAxisRect);
            maxArrow->start->setAxes(mXBottomMovingAxis, mYRightAxis);
            maxArrow->end->setAxisRect(mAxisRect);
            maxArrow->end->setAxes(mXBottomMovingAxis, mYRightAxis);
            maxArrow->setClipAxisRect(mAxisRect);
            maxArrow->setClipToAxisRect(false);
            maxArrow->setPen(graph->pen());
            maxArrow->setVisible(maxVisible);

            mStatMaxArrows.append(maxArrow);

            auto maxText = new QCPItemText(mBoard);
            maxText->position->setAxisRect(mAxisRect);
            maxText->position->setParentAnchor(maxArrow->start);
            maxText->position->setAxisRect(mAxisRect);
            maxText->position->setAxes(mXBottomMovingAxis, mYRightAxis);
            maxText->setClipAxisRect(mAxisRect);
            maxText->setClipToAxisRect(false);
            maxText->setPadding(QMargins(8, 0, 8, 0));
            maxText->setPen(graph->pen());
            maxText->setColor(colorSet.foregroundColor());
            maxText->setBrush(colorSet.backgroundBrush());
            maxText->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            maxText->setVisible(maxVisible);

            mStatMaxTexts.append(maxText);

            auto minTracer = new QCPItemTracer(mBoard);
            minTracer->position->setAxes(mXBottomMovingAxis, mYRightAxis);
            minTracer->position->setAxisRect(mAxisRect);
            minTracer->position->setType(QCPItemPosition::ptPlotCoords);
            minTracer->setClipAxisRect(mAxisRect);
            minTracer->setClipToAxisRect(true);
            minTracer->setGraph(graph);
            minTracer->setStyle(QCPItemTracer::tsCircle);
            minTracer->setPen(graph->pen());
            minTracer->setBrush(graph->pen().color());
            minTracer->setSize(7);
            minTracer->setVisible(minVisible);
            minTracer->setInterpolating(false);

            mStatMinTracers.append(minTracer);

            auto minArrow = new QCPItemLine(mBoard);
            minArrow->end->setParentAnchor(minTracer->position);
            minArrow->start->setParentAnchor(minArrow->end);
            minArrow->start->setCoords(15, -15);
            minArrow->start->setAxisRect(mAxisRect);
            minArrow->start->setAxes(mXBottomMovingAxis, mYRightAxis);
            minArrow->end->setAxisRect(mAxisRect);
            minArrow->end->setAxes(mXBottomMovingAxis, mYRightAxis);
            minArrow->setClipAxisRect(mAxisRect);
            minArrow->setClipToAxisRect(false);
            minArrow->setPen(graph->pen());
            minArrow->setVisible(minVisible);

            mStatMinArrows.append(minArrow);

            auto minText = new QCPItemText(mBoard);
            minText->position->setAxisRect(mAxisRect);
            minText->position->setParentAnchor(minArrow->start);
            minText->position->setAxisRect(mAxisRect);
            minText->position->setAxes(mXBottomMovingAxis, mYRightAxis);
            minText->setClipAxisRect(mAxisRect);
            minText->setClipToAxisRect(false);
            minText->setPadding(QMargins(8, 0, 8, 0));
            minText->setPen(graph->pen());
            minText->setColor(colorSet.foregroundColor());
            minText->setBrush(colorSet.backgroundBrush());
            minText->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            minText->setVisible(minVisible);

            mStatMinTexts.append(minText);
        }
    }
}

void PlotTime::updateItems()
{
    if(mStatModeEnabled && mStatMode != smNone) {
        for(int i = 0; i<mAxisRect->graphs().count(); i++) {
            auto graph = mAxisRect->graphs().at(i);
            QCPGraphDataContainer::const_iterator begin = graph->data()->constBegin();
            QCPGraphDataContainer::const_iterator end = graph->data()->constEnd();

            if(mStatMode != smMin) {
                const QCPGraphData* maxData = std::max_element(begin,
                                                               end,
                                                               [](QCPGraphData a, QCPGraphData b) { return a.value < b.value; });
                double max = maxData->value;
                double max_key = maxData->key;

                mStatMaxTracers[i]->setGraphKey(max_key);
                mStatMaxTracers[i]->updatePosition();

                mStatMaxTexts[i]->setText(QString::number(max, 'f', 3));
                if(max > mStatMaxTracers[i]->position->valueAxis()->range().lower
                        && max < mStatMaxTracers[i]->position->valueAxis()->range().upper) {
                    mStatMaxTexts[i]->setVisible(true);
                    mStatMaxArrows[i]->setVisible(true);
                } else {
                    mStatMaxTexts[i]->setVisible(false);
                    mStatMaxArrows[i]->setVisible(false);
                }
            }

            if(mStatMode != smMax) {
                const QCPGraphData minData = *std::min_element(begin,
                                                               end,
                                                               [](const QCPGraphData &a, const QCPGraphData &b) {
                    return a.value < b.value;
                });

                double min = minData.value;
                double min_key = minData.key;

                mStatMinTracers[i]->setGraphKey(min_key);
                mStatMinTracers[i]->updatePosition();

                mStatMinTexts[i]->setText(QString::number(min, 'f', 3));
                if(min > mStatMinTracers[i]->position->valueAxis()->range().lower
                        && min < mStatMinTracers[i]->position->valueAxis()->range().upper) {
                    mStatMinTexts[i]->setVisible(true);
                    mStatMinArrows[i]->setVisible(true);
                } else {
                    mStatMinTexts[i]->setVisible(false);
                    mStatMinArrows[i]->setVisible(false);
                }
            }
        }
    }

    if(mTracer)
        mTracer->updateItems();
}

void PlotTime::updateGraphsStyle()
{
    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<BoardParameter> dashParam = boardParameter(i);
        if(dashParam) {
            if(i < mAxisRect->graphs().count()) {
                if(dashParam->parameterConfiguration()->itemColorMode() == ParameterConfiguration::icDynamic)
                    mAxisRect->graphs().at(i)->setPen(QPen(dashParam->getColor(),dashParam->parameterConfiguration()->penWidth()));
            }
        }
    }
}

void PlotTime::resetGraphsStyle()
{
    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<BoardParameter> dashParam = boardParameter(i);
        if(dashParam) {
            if(i < mAxisRect->graphs().count()) {

                if(dashParam->getTimeSeries().parameterId() > 0) {
                    if(dashParam->parameterConfiguration()->itemColorMode() == ParameterConfiguration::icCustom)
                        mAxisRect->graphs().at(i)->setPen(QPen(dashParam->parameterConfiguration()->itemStaticColor(),dashParam->parameterConfiguration()->penWidth()));
                    else
                        mAxisRect->graphs().at(i)->setPen(QPen(dashParam->parameterConfiguration()->defaultColorSettingsRef().color(),dashParam->parameterConfiguration()->penWidth()));

                    switch(dashParam->parameterConfiguration()->graphBrush()) {
                    case ParameterConfiguration::bsNone :
                    {
                        mAxisRect->graphs().at(i)->setBrush(Qt::NoBrush);
                    }
                        break;
                    case ParameterConfiguration::bsFilled :
                    {
                        QColor color = dashParam->parameterConfiguration()->defaultColorSettingsRef().color();
                        color.setAlpha(50);
                        mAxisRect->graphs().at(i)->setBrush(color);
                        PlotTimeGraph *graph = qobject_cast<PlotTimeGraph*>(mAxisRect->graphs().at(i));
                        if(graph)
                            graph->setFillStyle(PlotTimeGraph::fsZero);
                    }
                        break;
                    case ParameterConfiguration::bsGradient :
                        QColor color = dashParam->parameterConfiguration()->defaultColorSettingsRef().color();
                        color.setAlpha(200);
                        QLinearGradient gradient(0, 1, 0, 0);
                        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
                        gradient.setColorAt(1, color);
                        gradient.setColorAt(0.1, QColor::fromRgbF(0, 0, 0, 0));
                        gradient.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0));
                        QBrush brush(gradient);

                        mAxisRect->graphs().at(i)->setBrush(brush);
                        PlotTimeGraph *graph = qobject_cast<PlotTimeGraph*>(mAxisRect->graphs().at(i));
                        if(graph)
                            graph->setFillStyle(PlotTimeGraph::fsBottom);
                        break;
                    }
                    mAxisRect->graphs().at(i)->setLineStyle(dashParam->parameterConfiguration()->graphLineStyle());

                    mAxisRect->graphs().at(i)->setScatterStyle(dashParam->parameterConfiguration()->scatterStyle());
                } else {
                    mAxisRect->graphs().at(i)->setLineStyle(QCPGraph::lsNone);
                    mAxisRect->graphs().at(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
                    mAxisRect->graphs().at(i)->setBrush(Qt::NoBrush);
                    mAxisRect->graphs().at(i)->setPen(Qt::NoPen);
                }
            }
        }
    }
}

void PlotTime::resetElement()
{
    for(auto el: mLayout->elements(false))
        mLayout->take(el);
    for(auto el: mMainLayout->elements(false))
        mMainLayout->take(el);
    mLegendLayout->setVisible(false);
    mLayout->simplify();
    mMainLayout->simplify();
}


bool PlotTime::xAxisFixedTicker() const
{
    return mXAxisFixedTicker;
}

void PlotTime::setXAxisFixedTicker(bool xAxisFixedTicker)
{
    mXAxisFixedTicker = xAxisFixedTicker;

    mXBottomStaticAxis->setVisible(mXAxisFixedTicker);
    mXBottomMovingAxis->setVisible(!mXAxisFixedTicker);
}

void PlotTime::buildElement()
{
    for(int i=0;i<mLegendLayout->elementCount();i++) {
        auto * element = qobject_cast<NumericalDisplay *>(mLegendLayout->elementAt(i));
        if(element) {
            element->rebuildElement();
            element->setOrientation(NumericalDisplay::doVerticalAlignCenter);
            element->setBackgroundMode(backgroundMode());
        }
    }

    switch(mLegendPosition) {
    case lpRight:
        mLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        mLayout->addElement(mAxisRect);

        if(mLegendLayout->elementCount() > 2 && mLegendVisible) {
            mLayout->addElement(mLegendLayout);
            mLegendLayout->setVisible(true);
            mLayout->setColumnStretchFactor(0,1);
            mLayout->setColumnStretchFactor(1,0.01);
        }

        mLegendLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
        mLegendLayout->setRowStretchFactor(0,0.01);
        mLegendLayout->setRowStretchFactor(mLegendLayout->elementCount() - 1,0.01);

        break;
    case lpLeft:
        mLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        if(mLegendLayout->elementCount() > 2 && mLegendVisible) {
            mLayout->addElement(mLegendLayout);
            mLayout->addElement(mAxisRect);
            mLayout->setColumnStretchFactor(0,0.01);
            mLayout->setColumnStretchFactor(1,1);

            mLegendLayout->setVisible(true);
        } else {
            mLayout->addElement(mAxisRect);
        }

        mLegendLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
        mLegendLayout->setRowStretchFactor(0,0.01);
        mLegendLayout->setRowStretchFactor(mLegendLayout->elementCount() - 1,0.01);
        break;
    case lpBottom:
        mLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
        mLayout->addElement(mAxisRect);
        if(mLegendLayout->elementCount() > 2 && mLegendVisible) {
            mLayout->addElement(mLegendLayout);
            mLegendLayout->setVisible(true);
            mLayout->setRowStretchFactor(0,1);
            mLayout->setRowStretchFactor(1,0.01);
        }

        mLegendLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        mLegendLayout->setColumnStretchFactor(0,0.01);
        mLegendLayout->setColumnStretchFactor(mLegendLayout->elementCount() - 1,0.01);
        break;
    case lpTop:
        mLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);

        if(mLegendLayout->elementCount() > 2 && mLegendVisible) {
            mLayout->addElement(mLegendLayout);
            mLayout->addElement(mAxisRect);
            mLayout->setRowStretchFactor(0,0.01);
            mLayout->setRowStretchFactor(1,1);
            mLegendLayout->setVisible(true);
        } else {
            mLayout->addElement(mAxisRect);
        }

        mLegendLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        mLegendLayout->setColumnStretchFactor(0,0.01);
        mLegendLayout->setColumnStretchFactor(mLegendLayout->elementCount() - 1,0.01);
        break;
    }

    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mTitleLabel->setTextFlags(Qt::AlignCenter);
    mTitleLabel->setText(mTitle);
    mTitleLabel->setVisible(mTitleVisible);
    mTitleLabel->needUpdate(true);


    if(mTitleVisible) {
        mMainLayout->addElement(mTitleLabel);
        mMainLayout->setRowStretchFactor(0,0.001);
    }

    mMainLayout->addElement(mLayout);
    mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);
    mMainLayout->setRowSpacing(0);

    if(mYAxisLabelsVisible || mYAxisGridVisible || mYAxisTicksVisible)
        mYRightAxis->setVisible(true);
    else
        mYRightAxis->setVisible(false);


    auto textColor = QApplication::palette().toolTipText().color();
    if(mXAxisLabelsVisible || mXAxisGridVisible || mXAxisTicksVisible) {
        mXBottomMovingAxis->setVisible(true & !mXAxisFixedTicker);
        mXBottomStaticAxis->setVisible(true & mXAxisFixedTicker);
    } else {
        mXBottomMovingAxis->setVisible(false);
        mXBottomStaticAxis->setVisible(false);
    }

    if(mYAxisLineVisible) {
        mYRightAxis->setBasePen(QPen(textColor, 0));
    } else {
        mYRightAxis->setBasePen(Qt::NoPen);
    }

    if(mXAxisLineVisible) {
        mXBottomMovingAxis->setBasePen(QPen(textColor, 0));
        mXBottomStaticAxis->setBasePen(QPen(textColor, 0));
    } else {
        mXBottomMovingAxis->setBasePen(Qt::NoPen);
        mXBottomStaticAxis->setBasePen(Qt::NoPen);
    }

    if(mYAxisLabelsVisible)
        mAxisRect->setMinimumMargins(QMargins(22,10,50,10));
    else
        mAxisRect->setMinimumMargins(QMargins(22,10,22,10));

    if(mYAxisTicksVisible) {
        mYRightAxis->setTickPen(QPen(textColor, 0));
        mYRightAxis->setSubTickPen(QPen(textColor, 0));
    } else {
        mYRightAxis->setTickPen(Qt::NoPen);
        mYRightAxis->setSubTickPen(Qt::NoPen);
    }

    mYRightAxis->setTickLabels(mYAxisLabelsVisible);
    mYRightAxis->ticker()->setTickCount(mYAxisTicksCount);
    mYRightAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

    mYRightAxis->grid()->setVisible(mYAxisGridVisible);

    if(mXAxisTicksVisible) {
        mXBottomMovingAxis->setTickPen(QPen(textColor, 0));
        mXBottomMovingAxis->setSubTickPen(QPen(textColor, 0));
        mXBottomStaticAxis->setTickPen(QPen(textColor, 0));
        mXBottomStaticAxis->setSubTickPen(QPen(textColor, 0));
    } else {
        mXBottomMovingAxis->setTickPen(Qt::NoPen);
        mXBottomMovingAxis->setSubTickPen(Qt::NoPen);
        mXBottomStaticAxis->setTickPen(Qt::NoPen);
        mXBottomStaticAxis->setSubTickPen(Qt::NoPen);
    }

    mXBottomMovingAxis->setTickLabels(mXAxisLabelsVisible);
    mXBottomStaticAxis->setTickLabels(mXAxisLabelsVisible);
    mXBottomMovingAxis->grid()->setVisible(mXAxisGridVisible);
    mXBottomStaticAxis->grid()->setVisible(mXAxisGridVisible);

    if(mXAxisDirection == adRightToLeft) {
        mXBottomMovingAxis->setRangeReversed(false);
        mXBottomStaticAxis->setRangeReversed(false);
    } else {
        mXBottomMovingAxis->setRangeReversed(true);
        mXBottomStaticAxis->setRangeReversed(true);
    }

    mXBottomMovingAxis->ticker()->setTickCount(mXAxisTicksCount);
    mXBottomMovingAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    mXBottomStaticAxis->ticker()->setTickCount(mXAxisTicksCount);
    mXBottomStaticAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

    mLegendLayout->needUpdate(true);
    mLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void PlotTime::update(QCPLayoutElement::UpdatePhase phase)
{
    BoardElement::update(phase);
    if(phase == upLayout) {
        mMainLayout->setOuterRect(rect());
        applySizeConstraints();
    }
    mMainLayout->update(phase);
}

bool PlotTime::thresholdsVisible() const
{
    return mThresholdsVisible;
}

void PlotTime::setThresholdsVisible(bool thresholdsVisible)
{
    mThresholdsVisible = thresholdsVisible;
}

bool PlotTime::boardReferenceTimeActive() const
{
    return mBoardReferenceTimeActive;
}

void PlotTime::setBoardReferenceTimeActive(bool xAxisLocalTimeReference)
{
    mBoardReferenceTimeActive = xAxisLocalTimeReference;
}

PlotTime::StatMode PlotTime::statMode() const
{
    return mStatMode;
}

void PlotTime::setStatMode(const StatMode &statMode)
{
    mStatMode = statMode;
}

bool PlotTime::yAxisLineVisible() const
{
    return mYAxisLineVisible;
}

void PlotTime::setYAxisLineVisible(bool yAxisLineVisible)
{
    mYAxisLineVisible = yAxisLineVisible;
}

bool PlotTime::xAxisLineVisible() const
{
    return mXAxisLineVisible;
}

void PlotTime::setXAxisLineVisible(bool xAxisLineVisible)
{
    mXAxisLineVisible = xAxisLineVisible;
}

PlotTime::LegendPosition PlotTime::legendPosition() const
{
    return mLegendPosition;
}

void PlotTime::setLegendPosition(const LegendPosition &legendPosition)
{
    mLegendPosition = legendPosition;
}

bool PlotTime::legendVisible() const
{
    return mLegendVisible;
}

void PlotTime::setLegendVisible(bool legendVisible)
{
    mLegendVisible = legendVisible;
}

bool PlotTime::yAxisTicksVisible() const
{
    return mYAxisTicksVisible;
}

void PlotTime::setYAxisTicksVisible(bool yAxisTicksVisible)
{
    mYAxisTicksVisible = yAxisTicksVisible;
}

bool PlotTime::yAxisLabelsVisible() const
{
    return mYAxisLabelsVisible;
}

void PlotTime::setYAxisLabelsVisible(bool yAxisLabelsVisible)
{
    mYAxisLabelsVisible = yAxisLabelsVisible;
}

bool PlotTime::yAxisGridVisible() const
{
    return mYAxisGridVisible;
}

void PlotTime::setYAxisGridVisible(bool yAxisGridVisible)
{
    mYAxisGridVisible = yAxisGridVisible;
}

PlotTime::YAxisScale PlotTime::yAxisScale() const
{
    return mYAxisScale;
}

void PlotTime::setYAxisScale(const YAxisScale &yAxisScale)
{
    mYAxisScale = yAxisScale;
}

double PlotTime::yAxisMaxCustom() const
{
    return mYAxisMaxCustom;
}

void PlotTime::setYAxisMaxCustom(double yAxisMaxCustom)
{
    mYAxisMaxCustom = yAxisMaxCustom;
}

double PlotTime::yAxisMinCustom() const
{
    return mYAxisMinCustom;
}

void PlotTime::setYAxisMinCustom(double yAxisMinCustom)
{
    mYAxisMinCustom = yAxisMinCustom;
}

bool PlotTime::xAxisTicksVisible() const
{
    return mXAxisTicksVisible;
}

void PlotTime::setXAxisTicksVisible(bool xAxisTicksVisible)
{
    mXAxisTicksVisible = xAxisTicksVisible;
}

bool PlotTime::xAxisLabelsVisible() const
{
    return mXAxisLabelsVisible;
}

void PlotTime::setXAxisLabelsVisible(bool xAxisLabelsVisible)
{
    mXAxisLabelsVisible = xAxisLabelsVisible;
}

bool PlotTime::xAxisGridVisible() const
{
    return mXAxisGridVisible;
}

void PlotTime::setXAxisGridVisible(bool xAxisGridVisible)
{
    mXAxisGridVisible = xAxisGridVisible;
}

double PlotTime::xAxisHistory() const
{
    return mXAxisHistory;
}

void PlotTime::setXAxisHistory(double xAxisHistory)
{
    if(xAxisHistory > mXAxisHistory)
        loadHistoricalData();
    mXAxisHistory = xAxisHistory;
}

PlotTime::XAxisDirection PlotTime::xAxisDirection() const
{
    return mXAxisDirection;
}

void PlotTime::setXAxisDirection(const XAxisDirection &xAxisDirection)
{
    mXAxisDirection = xAxisDirection;
}

int PlotTime::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int PlotTime::defaultHeight()
{
    return 3*mBoard->dashboardLayout()->singleElementRowCount();
}

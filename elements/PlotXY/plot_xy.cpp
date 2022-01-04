#include "plot_xy.h"
#include <utility>
#include "plot_xy_editor.h"

PlotXY::PlotXY(Board *dashboard) :
    BoardElement (dashboard),
    mTitleVisible(true),
    mTitle("Curves"),
    mYLegendPosition(lpLeft),
    mYAxisScale(asAuto),
    mYLegendVisible(true),
    mXLegendPosition(lpBottom),
    mXAxisScale(asAuto),
    mXLegendVisible(true),
    mYAxisTicksVisible(false),
    mYAxisTicksCount(5),
    mYAxisLineVisible(false),
    mYAxisLabelsVisible(true),
    mYAxisGridVisible(true),
    mXAxisTicksVisible(false),
    mXAxisTicksCount(5),
    mXAxisLineVisible(false),
    mXAxisLabelsVisible(true),
    mXAxisGridVisible(true),
    mXAxisHistory(10),
    mYThresholdsVisible(true),
    mXThresholdsVisible(true),
    mPlotInRangePoints(false),
    mXParameter(nullptr),
    mXDisplay(nullptr)
{
    setParametersMaxCount(9);
    setConfigurationMode(ParameterConfiguration::cmCurveX);
}

void PlotXY::clearElement()
{
    for(auto  element : mYDisplays)
        element->clearElement();

    if(mXDisplay)
        mXDisplay->clearElement();

    QList<QCPAbstractItem*> items = mAxisRect->items();
    for(int i=0; i<items.count();i++)
        mBoard->removeItem(items.at(i));

    mAxisRect->clearPlottables();
    mYLegendLayout->clear();
    mXLegendLayout->clear();
    mLayout->clear();
    mMainLayout->clear();
    delete mMainLayout;
}


void PlotXY::initialize(Board *dashboard)
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

        textColor = QApplication::palette().toolTipText().color();
        mAxisRect = new AxisRect(dashboard);
        mAxisRect->setAxisColor(textColor);
        mAxisRect->axis(QCPAxis::atLeft)->setVisible(true);
        mAxisRect->axis(QCPAxis::atRight)->setVisible(false);
        connect(mAxisRect->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)),
                mAxisRect->axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));
        connect(mAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),
                mAxisRect->axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));

        mYLegendLayout = new LayoutGrid();
        mYLegendLayout->initializeParentPlot(dashboard);
        mYLegendLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
        mYLegendLayout->setRowSpacing(1);
        mYLegendLayout->setMinimumSize(50,50);

        mYLegendEmptyElementTop = new QCPLayoutElement(dashboard);
        mYLegendEmptyElementTop->setMinimumSize(0.1,0.1);
        mYLegendEmptyElementBottom = new QCPLayoutElement(dashboard);
        mYLegendEmptyElementBottom->setMinimumSize(0.1,0.1);
        mYLegendLayout->addElement(mYLegendEmptyElementTop);
        mYLegendLayout->addElement(mYLegendEmptyElementBottom);
        mYLegendLayout->setRowStretchFactor(0,0.01);

        mXLegendLayout = new LayoutGrid();
        mXLegendLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        mXLegendLayout->initializeParentPlot(dashboard);
        mXLegendLayout->setColumnSpacing(1);
        mXLegendLayout->setMinimumSize(50,50);

        mXDisplay = new NumericalDisplay(dashboard);
        mXDisplay->initialize(dashboard);
        mXDisplay->setMainElement(false);
        mXDisplay->setBackgroundMode(backgroundMode());

        mXLegendEmptyElementLeft = new QCPLayoutElement(dashboard);
        mXLegendEmptyElementLeft->setMinimumSize(0.1,0.1);
        mXLegendEmptyElementRight = new QCPLayoutElement(dashboard);
        mXLegendEmptyElementRight->setMinimumSize(0.1,0.1);
        mXLegendLayout->addElement(mXLegendEmptyElementLeft);
        mXLegendLayout->addElement(mXDisplay);
        mXLegendLayout->addElement(mXLegendEmptyElementRight);
        mXLegendLayout->setColumnStretchFactor(0,0.01);
        mXLegendLayout->setColumnStretchFactor(2,0.01);
    }
}

void PlotXY::edit()
{
    PlotXYEditor editor(this);
    editor.exec();
}

void PlotXY::loadSettings(QSettings *settings)
{
    settings->beginGroup("General");
    if(settings->contains("YLegendVisible"))
        setYLegendVisible(settings->value("YLegendVisible").toBool());
    if(settings->contains("YLegendPosition"))
        setYLegendPosition(PlotXY::YLegendPosition(settings->value("YLegendPosition").toInt()));
    if(settings->contains("XLegendVisible"))
        setXLegendVisible(settings->value("XLegendVisible").toBool());
    if(settings->contains("XLegendPosition"))
        setXLegendPosition(PlotXY::XLegendPosition(settings->value("XLegendPosition").toInt()));

    mTitleVisible = settings->value("TitleVisible", false).toBool();
    mTitle = settings->value("Title", "Curves").toString();
    if(settings->contains("BackgroundMode")) {
        auto maxVal = QMetaEnum::fromType<BackgroundMode>().keyCount();
        auto val = settings->value("BackgroundMode").toInt();
        if(val <   maxVal)
            setBackgroundMode(BackgroundMode(val));
    }

    if(settings->contains("PlotInRange"))
        setPlotInRangePoints(settings->value("PlotInRange").toBool());
    settings->endGroup();

    settings->beginGroup("YAxis");
    if(settings->contains("TicksVisible"))
        setYAxisTicksVisible(settings->value("TicksVisible").toBool());
    if(settings->contains("TicksCount"))
        setYAxisTicksCount(settings->value("TicksCount").toInt());
    if(settings->contains("LineVisible"))
        setYAxisLineVisible(settings->value("LineVisible").toBool());
    if(settings->contains("LabelsVisible"))
        setYAxisLabelsVisible(settings->value("LabelsVisible").toBool());
    if(settings->contains("GridVisible"))
        setYAxisGridVisible(settings->value("GridVisible").toBool());
    if(settings->contains("ScaleMode"))
        setYAxisScale(AxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setYAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setYAxisMaxCustom(settings->value("ScaleMax").toDouble());
    if(settings->contains("ThresholdsVisible"))
        setYThresholdsVisible(settings->value("ThresholdsVisible").toBool());
    settings->endGroup();

    settings->beginGroup("XAxis");
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
    if(settings->contains("ScaleMode"))
        setXAxisScale(AxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setXAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setXAxisMaxCustom(settings->value("ScaleMax").toDouble());
    if(settings->contains("History"))
        setXAxisHistory(settings->value("History").toInt());
    if(settings->contains("ThresholdsVisible"))
        setXThresholdsVisible(settings->value("ThresholdsVisible").toBool());
    settings->endGroup();

    mPatrons.clear();
    int count = settings->beginReadArray("Patrons");
    for(int i=0; i< count;i++) {
        settings->setArrayIndex(i);
        QSharedPointer<CurvePatron> patron(new CurvePatron());
        patron->load(settings);
        mPatrons.append(patron);
    }
    settings->endArray();

}

void PlotXY::loadConfigurations(QSettings *settings)
{
    mXParameter = nullptr;
    if(settings->childGroups().contains("XParameter")) {
        settings->beginGroup("XParameter");

        mXParameter =  QSharedPointer<BoardParameter>(new BoardParameter(mBoard));
        mXParameter->loadParameterSettings(settings, ParameterConfiguration::cmCurveX);
        mBoardParameters.append(mXParameter);

        mXDisplay->addParameter(mXParameter);
        settings->endGroup();
    }

    int paramCount = settings->beginReadArray("Parameters");

    for(int i= 0; i< paramCount;i++) {
        settings->setArrayIndex(i);
        QSharedPointer<BoardParameter> dashParam =  QSharedPointer<BoardParameter>(new BoardParameter(mBoard));
        mBoardParameters.append(dashParam);
        mYParameters.append(dashParam);
        dashParam->loadParameterSettings(settings, ParameterConfiguration::cmCurveY);

        auto legend = new NumericalDisplay(mBoard);
        legend->initialize(mBoard);
        legend->addParameter(dashParam);
        legend->setBackgroundMode(backgroundMode());
        legend->setMainElement(false);
        legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);

        mYDisplays.append(legend);

        mYLegendLayout->take(mYLegendEmptyElementBottom);
        mYLegendLayout->simplify();
        mYLegendLayout->addElement(legend);
        mYLegendLayout->addElement(mYLegendEmptyElementBottom);
        mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);

        auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
        mCurves.append(curve);
    }
    settings->endArray();

    checkParameters();
    rebuildElement();
}

void PlotXY::checkParameters()
{
    if(mXDisplay)
        mXDisplay->checkParameters();
    for(auto disp : mYDisplays)
        disp->checkParameters();

    int yParamSize = mYParameters.count();
    for(int i=0; i< yParamSize; i++) {
        QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
        if(dashParam && dashParam->getTimeSeries().parameterId() == 0) {
            if(i < mCurves.count())
                mCurves.at(i)->data()->clear();
            if(i < mHLines.count())
                mHLines.at(i)->setVisible(false);
            if(i < mTracers.count())
                mTracers.at(i)->setVisible(false);
            if(i < mVLines.count())
                mVLines.at(i)->setVisible(false);
        } else {
            if(mXParameter && mXParameter->getTimeSeries().parameterId() > 0) {
                if(i < mHLines.count())
                    mHLines.at(i)->setVisible(true);
                if(i < mTracers.count())
                    mTracers.at(i)->setVisible(true);
                if(i < mVLines.count())
                    mVLines.at(i)->setVisible(true);
            }
        }
    }
}

void PlotXY::saveSettings(QSettings *settings)
{
    settings->beginGroup("General");
    settings->setValue("YLegendVisible", mYLegendVisible);
    settings->setValue("YLegendPosition", mYLegendPosition);
    settings->setValue("XLegendVisible", mXLegendVisible);
    settings->setValue("XLegendPosition", mXLegendPosition);
    settings->setValue("BackgroundMode", mBackgroundMode);
    settings->setValue("TitleVisible", mTitleVisible);
    settings->setValue("Title", mTitle);
    settings->setValue("PlotInRange", mPlotInRangePoints);
    settings->endGroup();

    settings->beginGroup("YAxis");
    settings->setValue("TicksVisible", mYAxisTicksVisible);
    settings->setValue("TicksCount", mYAxisTicksCount);
    settings->setValue("LineVisible", mYAxisLineVisible);
    settings->setValue("LabelsVisible", mYAxisLabelsVisible);
    settings->setValue("GridVisible", mYAxisGridVisible);
    settings->setValue("ScaleMode", mYAxisScale);
    settings->setValue("ScaleMin", mYAxisMinCustom);
    settings->setValue("ScaleMax", mYAxisMaxCustom);
    settings->setValue("ThresholdsVisible", mYThresholdsVisible);
    settings->endGroup();

    settings->beginGroup("XAxis");
    settings->setValue("TicksVisible", mXAxisTicksVisible);
    settings->setValue("TicksCount", mXAxisTicksCount);
    settings->setValue("LineVisible", mXAxisLineVisible);
    settings->setValue("LabelsVisible", mXAxisLabelsVisible);
    settings->setValue("GridVisible", mXAxisGridVisible);
    settings->setValue("ScaleMode", mXAxisScale);
    settings->setValue("ScaleMin", mXAxisMinCustom);
    settings->setValue("ScaleMax", mXAxisMaxCustom);
    settings->setValue("History", mXAxisHistory);
    settings->setValue("ThresholdsVisible", mXThresholdsVisible);
    settings->endGroup();

    settings->beginWriteArray("Patrons");
    for(int i=0; i< mPatrons.count();i++) {
        settings->setArrayIndex(i);
        mPatrons.at(i)->save(settings);
    }
    settings->endArray();
}

void PlotXY::saveConfigurations(QSettings *settings)
{
    settings->beginWriteArray("Parameters");
    int count = -1;
    for(int i= 0; i< mYParameters.count();i++) {
        count ++;
        settings->setArrayIndex(count);
        mYParameters.at(i)->saveParameterSettings(settings, ParameterConfiguration::cmCurveY);
    }
    settings->endArray();

    if(mXParameter) {
        settings->beginGroup("XParameter");
        mXParameter->saveParameterSettings(settings, ParameterConfiguration::cmCurveX);
        settings->endGroup();
    } else {
        settings->remove("XParameter");
    }
}

void PlotXY::droppedParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    QStringList items;
    items << tr("Y Axis") << tr("X Axis");

    bool ok;
    QString item = QInputDialog::getItem(nullptr, name(),
                                         tr("Axis:"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        if(item == QString("Y Axis")) {
            addYParameter(parameterSettings);
        } else {
            addXParameter(parameterSettings);
        }
    }
}

void PlotXY::droppedParameter(QSharedPointer<TimeSeries> dataParameter, bool replace)
{
    Q_UNUSED(replace)
    QStringList items;
    items << tr("Y Axis") << tr("X Axis");

    bool ok;
    QString item = QInputDialog::getItem(nullptr, name(),
                                         tr("Axis:"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        if(item == QString("Y Axis")) {
            addYParameter(dataParameter);
        } else {
            addXParameter(dataParameter);
        }
    }
}

QSharedPointer<BoardParameter> PlotXY::addYParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    BoardElement::addParameter(parameterSettings);

    auto legend = new NumericalDisplay(mBoard);
    legend->initialize(mBoard);
    legend->setBackgroundMode(backgroundMode());
    legend->setMainElement(false);
    legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
    auto dashParam = legend->addParameter(std::move(parameterSettings));
    mYParameters.append(dashParam);
    mYDisplays.append(legend);

    mYLegendLayout->take(mYLegendEmptyElementBottom);
    mYLegendLayout->simplify();
    mYLegendLayout->addElement(legend);
    mYLegendLayout->addElement(mYLegendEmptyElementBottom);
    mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);
    mYLegendLayout->needUpdate(true);

    auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
    mCurves.append(curve);

    rebuildElement();

    return dashParam;
}

QSharedPointer<BoardParameter> PlotXY::addYParameter(QString paramLabel)
{
    BoardElement::addParameter(paramLabel);
    auto legend = new NumericalDisplay(mBoard);
    legend->initialize(mBoard);
    legend->setBackgroundMode(backgroundMode());
    legend->setMainElement(false);
    legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
    auto dashParam = legend->addParameter(std::move(paramLabel));
    mYParameters.append(dashParam);
    mYDisplays.append(legend);

    mYLegendLayout->take(mYLegendEmptyElementBottom);
    mYLegendLayout->simplify();
    mYLegendLayout->addElement(legend);
    mYLegendLayout->addElement(mYLegendEmptyElementBottom);
    mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);
    mYLegendLayout->needUpdate(true);

    auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
    mCurves.append(curve);

    rebuildElement();

    return dashParam;
}

QSharedPointer<BoardParameter> PlotXY::addYParameter(QSharedPointer<TimeSeries> dataParameter)
{
    BoardElement::addParameter(dataParameter);
    auto legend = new NumericalDisplay(mBoard);
    legend->initialize(mBoard);
    legend->setBackgroundMode(backgroundMode());
    legend->setMainElement(false);
    legend->setOrientation(NumericalDisplay::doVerticalAlignCenter);
    auto dashParam = legend->addParameter(dataParameter);
    mYParameters.append(dashParam);
    mYDisplays.append(legend);

    mYLegendLayout->take(mYLegendEmptyElementBottom);
    mYLegendLayout->simplify();
    mYLegendLayout->addElement(legend);
    mYLegendLayout->addElement(mYLegendEmptyElementBottom);
    mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);
    mYLegendLayout->needUpdate(true);

    auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
    mCurves.append(curve);

    rebuildElement();

    return dashParam;
}

QSharedPointer<BoardParameter> PlotXY::addXParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{    
    BoardElement::addParameter(parameterSettings);
    mXParameter = mXDisplay->addParameter(std::move(parameterSettings));
    rebuildElement();

    for(int i=0; i< mCurves.count();i++)
        mCurves.at(i)->data()->clear();

    return mXParameter;
}

QSharedPointer<BoardParameter> PlotXY::addXParameter(QString paramLabel)
{
    BoardElement::addParameter(paramLabel);
    mXParameter = mXDisplay->addParameter(std::move(paramLabel));
    rebuildElement();

    for(int i=0; i< mCurves.count();i++)
        mCurves.at(i)->data()->clear();

    return mXParameter;
}

QSharedPointer<BoardParameter> PlotXY::addXParameter(const QSharedPointer<TimeSeries>& dataParameter)
{
    BoardElement::addParameter(dataParameter);
    mXParameter = mXDisplay->addParameter(dataParameter);
    rebuildElement();

    for(int i=0; i< mCurves.count();i++)
        mCurves.at(i)->data()->clear();

    return mXParameter;
}

void PlotXY::removeYParameter(int index, bool update)
{
    BoardElement::removeBoardParameter(index, update);
    QCPCurve *curve = mCurves.takeAt(index);
    mBoard->removePlottable(curve);

    QCPItemStraightLine *vLine = mVLines.takeAt(index);
    mBoard->removeItem(vLine);

    QCPItemStraightLine *hLine = mHLines.takeAt(index);
    mBoard->removeItem(hLine);

    QCPItemTracer *tracer = mTracers.takeAt(index);
    mBoard->removeItem(tracer);

    QSharedPointer<BoardParameter> dashParam = mYParameters.takeAt(index);
    mBoardParameters.removeAll(dashParam);
    mParametersLabel.removeAll(dashParam->getTimeSeries().name());

    auto disp = mYDisplays.takeAt(index);
    disp->removeBoardParameters();

    QCPLayoutElement *el = mYLegendLayout->takeAt(index + 1);
    mYLegendLayout->simplify();
    mYLegendLayout->needUpdate(true);
    auto  element = qobject_cast<NumericalDisplay *>(el);
    if(element)
        element->clearElement();
    el->deleteLater();

    if(update)
        rebuildElement();
}

void PlotXY::removeXParameter()
{    
    if(mXParameter) {
        BoardElement::removeBoardParameter(mXParameter->getTimeSeries().name(), false);
        mXDisplay->removeBoardParameters();

        mXParameter.reset();
        mXParameter = nullptr;

        rebuildElement();
    }
}

void PlotXY::removeBoardParameters()
{
    for(int i= mYParameters.count() -1; i>=0;i--)
        removeYParameter(i, false);
    removeXParameter();

    mYParameters.clear();
    mParametersLabel.clear();
    mBoardParameters.clear();
    rebuildElement();
}

void PlotXY::resetElement()
{
    for(auto el: mLayout->elements(false))
        if(el)
            mLayout->take(el);
    for(auto el: mMainLayout->elements(false))
        mMainLayout->take(el);

    mXLegendLayout->setVisible(false);
    mYLegendLayout->setVisible(false);
    mLayout->simplify();
    mMainLayout->simplify();

    for(int i=0;i<mYLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<NumericalDisplay *>(mYLegendLayout->elementAt(i));
        if(element) {
            element->resetElement();
        }
    }

    for(int i=0;i<mXLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<NumericalDisplay *>(mXLegendLayout->elementAt(i));
        if(element) {
            element->resetElement();
        }
    }
}

void PlotXY::buildElement()
{
    mLayout->expandTo(2,2);

    switch(mXLegendPosition) {
    case lpBottom:
        switch(mYLegendPosition) {
        case lpRight:
            mLayout->addElement(0,0, mAxisRect);

            if(mYLegendLayout->elementCount() > 2 && mYLegendVisible) {
                mLayout->addElement(0,1, mYLegendLayout);
                mYLegendLayout->setVisible(true);
            } else {
                mYLegendLayout->setVisible(false);
            }

            if(mXLegendVisible) {
                mLayout->addElement(1,0, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mXLegendLayout->setVisible(false);
            }

            mLayout->setColumnStretchFactor(1,0.01);
            mLayout->setColumnStretchFactor(0,1);
            mLayout->setRowStretchFactor(0,1);
            mLayout->setRowStretchFactor(1,0.01);
            break;
        case lpLeft:
            if(mYLegendLayout->elementCount() > 2 && mYLegendVisible) {
                mLayout->addElement(0,0, mYLegendLayout);
                mYLegendLayout->setVisible(true);
            } else {
                mYLegendLayout->setVisible(false);
            }

            mLayout->addElement(0,1, mAxisRect);

            if(mXLegendVisible) {
                mLayout->addElement(1,1, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mXLegendLayout->setVisible(false);
            }

            mLayout->setColumnStretchFactor(0,0.01);
            mLayout->setColumnStretchFactor(1,1);
            mLayout->setRowStretchFactor(0,1);
            mLayout->setRowStretchFactor(1,0.01);
            break;
        }
        break;
    case lpTop:
        switch(mYLegendPosition) {
        case lpRight:
            if(mXLegendVisible) {
                mLayout->addElement(0,0, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mXLegendLayout->setVisible(false);
            }

            mLayout->addElement(1,0, mAxisRect);

            if(mYLegendLayout->elementCount() > 2 && mYLegendVisible) {
                mLayout->addElement(1,1, mYLegendLayout);
                mYLegendLayout->setVisible(true);
            } else {
                mYLegendLayout->setVisible(false);
            }

            mLayout->setColumnStretchFactor(1,0.01);
            mLayout->setColumnStretchFactor(0,1);
            mLayout->setRowStretchFactor(1,1);
            mLayout->setRowStretchFactor(0,0.01);
            break;
        case lpLeft:
            if(mXLegendVisible) {
                mLayout->addElement(0,1, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mXLegendLayout->setVisible(false);
            }

            if(mYLegendLayout->elementCount() > 2 && mYLegendVisible) {
                mLayout->addElement(1,0, mYLegendLayout);
                mYLegendLayout->setVisible(true);
            } else {
                mYLegendLayout->setVisible(false);
            }

            mLayout->addElement(1,1, mAxisRect);

            mLayout->setColumnStretchFactor(0,0.01);
            mLayout->setColumnStretchFactor(1,1);
            mLayout->setRowStretchFactor(1,1);
            mLayout->setRowStretchFactor(0,0.01);
            break;
        }
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

    for(int i=0;i<mYLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<NumericalDisplay *>(mYLegendLayout->elementAt(i));
        if(element) {
            element->buildElement();
            element->setOrientation(NumericalDisplay::doVerticalAlignCenter);
            element->setBackgroundMode(backgroundMode());
        }
    }

    for(int i=0;i<mXLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<NumericalDisplay *>(mXLegendLayout->elementAt(i));
        if(element) {
            element->buildElement();
            element->setOrientation(NumericalDisplay::doVerticalAlignCenter);
            element->setBackgroundMode(backgroundMode());
        }
    }

    auto textColor = QApplication::palette().toolTipText().color();
    if(mYAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(QPen(textColor, 0));
    } else {
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(Qt::NoPen);
    }

    mAxisRect->axis(QCPAxis::atLeft)->setTickLabels(mYAxisLabelsVisible);
    mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(mYAxisGridVisible);
    mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(mYAxisGridVisible);

    if(mXAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(QPen(textColor, 0));
    } else {
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);
    }

    mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(mXAxisLabelsVisible);
    mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(mXAxisGridVisible);

    if(mXAxisLineVisible) {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(QPen(textColor, 0));
    } else {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
    }

    if(mYAxisLineVisible) {
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(QPen(textColor, 0));
    } else {
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(Qt::NoPen);
    }

    mAxisRect->axis(QCPAxis::atLeft)->ticker()->setTickCount(mYAxisTicksCount);
    mAxisRect->axis(QCPAxis::atLeft)->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

    mAxisRect->axis(QCPAxis::atBottom)->ticker()->setTickCount(mXAxisTicksCount);
    mAxisRect->axis(QCPAxis::atBottom)->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

    applySizeConstraints();
    updateItems();
    updateGraphsStyle();

    mYLegendLayout->needUpdate(true);
    mXLegendLayout->needUpdate(true);
    mLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void PlotXY::loadData()
{
    for(auto element : mYDisplays)
        element->loadData();

    mXDisplay->loadData();

    if(mXParameter && mXParameter->getTimeSeries().parameterId() > 0) {
        int yParamSize = mYParameters.count();
        for(int i=0; i< yParamSize; i++) {
            QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
            if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {
                if(i < mCurves.count()) {
                    double valY = dashParam->getValueDouble() ;
                    double valX = mXParameter->getValueDouble();
                    if(mPlotInRangePoints) {
                        if(dashParam->parameterConfiguration()->validRange()) {
                            if(valY > dashParam->parameterConfiguration()->rangeMaximum() ||
                                    valY < dashParam->parameterConfiguration()->rangeMinimum())
                                valY = qQNaN();
                        }

                        if(mXParameter->parameterConfiguration()->validRange()) {
                            if(valX > mXParameter->parameterConfiguration()->rangeMaximum() ||
                                    valX < mXParameter->parameterConfiguration()->rangeMinimum())
                                valX = qQNaN();
                        }
                    }

                    mCurves.at(i)->addData(mXParameter->getTimestamp(), valX, valY);

                    if(mXAxisHistory > 0)
                        mCurves.at(i)->data()->removeBefore(mXParameter->getTimestamp()-mXAxisHistory);
                }
            }
        }
    }
}

void PlotXY::loadHistoricalData()
{
    if(mBoard->dataManager()) {

        for(auto element : mYDisplays)
            element->loadHistoricalData();

        mXDisplay->loadHistoricalData();

        if(mXParameter && mXParameter->getTimeSeries().parameterId() > 0) {
            int yParamSize = mYParameters.count();
            for(int i=0; i< yParamSize; i++) {
                QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
                if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {
                    if(i < mCurves.count()) {
                        double valY = dashParam->getValueDouble() ;
                        double valX = mXParameter->getValueDouble();

                        if(mPlotInRangePoints) {
                            if(dashParam->parameterConfiguration()->validRange()) {
                                if(valY > dashParam->parameterConfiguration()->rangeMaximum() ||
                                        valY < dashParam->parameterConfiguration()->rangeMinimum())
                                    valY = qQNaN();
                            }

                            if(mXParameter->parameterConfiguration()->validRange()) {
                                if(valX > mXParameter->parameterConfiguration()->rangeMaximum() ||
                                        valX < mXParameter->parameterConfiguration()->rangeMinimum())
                                    valX = qQNaN();
                            }
                        }


                        mCurves.at(i)->data()->clear();
                        mCurves.at(i)->addData(mXParameter->getTimestamp(), valX, valY);
                    }
                }
            }
        }

    }
}

void PlotXY::displayData()
{
    for(auto element : mYDisplays)
        element->displayData();

    mXDisplay->displayData();

    if(mXParameter && mXParameter->getTimeSeries().parameterId() > 0) {
        int yParamSize = mYParameters.count();
        for(int i=0; i< yParamSize; i++) {
            QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
            if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {
                if(i < mCurves.count()) {

                    double valY = dashParam->getValueDouble() ;
                    double valX = mXParameter->getValueDouble();

                    mTracers.at(i)->position->setCoords(valX, valY);

                    mHLines.at(i)->point1->setCoords(0, valY);
                    mHLines.at(i)->point2->setCoords(1, valY);

                    mVLines.at(i)->point1->setCoords(valX, 0.);
                    mVLines.at(i)->point2->setCoords(valX, 1.);
                }
            }
        }
    }

    updateAxes();
}

void PlotXY::applySizeConstraints()
{
    QSizeF size = mBoard->dashboardLayout()->singleElementSize();

    if(mYLegendVisible) {
        mYLegendLayout->setMinimumSize(size.width(), 50);
        for(int i=1; i< mYLegendLayout->elementCount() - 1;i++) {
            if( qobject_cast<NumericalDisplay *>(mYLegendLayout->elementAt(i))) {
                mYLegendLayout->elementAt(i)->setMinimumSize(size.width(), 0);
                mYLegendLayout->elementAt(i)->setMaximumSize(size.width(), size.height());
            }
        }
    }

    if(mXLegendVisible) {
        mXLegendLayout->setMinimumSize(50, size.height());
        auto  element = qobject_cast<NumericalDisplay *>(mXLegendLayout->elementAt(1));
        if(element) {
            element->setMinimumSize(0, size.height());
            element->setMaximumSize(size.width(), size.height());
        }
    }

    if(mTitleVisible) {
        size.rwidth() -= mMargins.left() + mMargins.right() + mMainLayout->margins().left() + mMainLayout->margins().right();
        size.rheight() -= mMargins.top() + mMargins.bottom() + mMainLayout->margins().top() + mMainLayout->margins().bottom();

        QSizeF currentSize = mRect.size();
        currentSize.rwidth() -= mMainLayout->margins().left() + mMainLayout->margins().right();
        currentSize.rheight() -= mMainLayout->margins().top() + mMainLayout->margins().bottom();

        QSizeF labelSize = size;

        if(currentSize.height() < size.height())
            labelSize = currentSize;

        mTitleLabel->setMinimumSize(labelSize.width(), 0.3 * labelSize.height());
    }
}

void PlotXY::updateAxes()
{
    switch(mYAxisScale) {
    case asAuto:
    {
        bool canRescale = false;
        for(int i=0; i< mYParameters.count(); i++) {
            QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
            if(dashParam) {
                if(dashParam->getTimeSeries().parameterId() > 0) {
                    canRescale = true;
                    break;
                }
            }
        }
        if(canRescale) {
            if(mXParameter) {
                if(mXParameter->getTimeSeries().parameterId() > 0) {
                    if(mAxisRect->plottables().count() > 0) {
                        mAxisRect->axis(QCPAxis::atLeft)->rescale();
                        mYAxisMinCustom = mAxisRect->axis(QCPAxis::atLeft)->range().lower;
                        mYAxisMaxCustom = mAxisRect->axis(QCPAxis::atLeft)->range().upper;
                    }
                }
            }
        }
    }
        break;
    case asParam:
    {
        bool validRange = false;
        bool canAutoRescale = false;
        for(int i=0; i< mYParameters.count(); i++) {
            QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
            if(dashParam) {
                if(dashParam->getTimeSeries().parameterId() > 0)
                    canAutoRescale = true;
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

        if(!validRange && canAutoRescale) {
            if(mAxisRect->plottables().count() > 0) {
                if(mXParameter) {
                    if(mXParameter->getTimeSeries().parameterId() > 0) {
                        mAxisRect->axis(QCPAxis::atLeft)->rescale();
                        mYAxisMinCustom = mAxisRect->axis(QCPAxis::atLeft)->range().lower;
                        mYAxisMaxCustom = mAxisRect->axis(QCPAxis::atLeft)->range().upper;
                    }
                }
            }
        } else {
            mAxisRect->axis(QCPAxis::atLeft)->setRange(QCPRange(mYAxisMinCustom, mYAxisMaxCustom));
        }
        break;
    }
    case asCustom:
        mAxisRect->axis(QCPAxis::atLeft)->setRange(QCPRange(mYAxisMinCustom, mYAxisMaxCustom));
        break;
    }

    switch(mXAxisScale) {
    case asAuto:
        if(mXParameter) {
            if(mXParameter->getTimeSeries().parameterId() > 0) {
                mAxisRect->axis(QCPAxis::atBottom)->rescale();
                mXAxisMinCustom = mAxisRect->axis(QCPAxis::atBottom)->range().lower;
                mXAxisMaxCustom = mAxisRect->axis(QCPAxis::atBottom)->range().upper;
            }
        }
        break;
    case asParam:
        if(mXParameter) {
            if(mXParameter->parameterConfiguration()->validRange()) {
                mAxisRect->axis(QCPAxis::atBottom)->setRange(QCPRange(mXParameter->parameterConfiguration()->rangeMinimum(), mXParameter->parameterConfiguration()->rangeMaximum()));
            } else if(mXParameter->getTimeSeries().parameterId() > 0) {
                mAxisRect->axis(QCPAxis::atBottom)->rescale();
                mXAxisMinCustom = mAxisRect->axis(QCPAxis::atBottom)->range().lower;
                mXAxisMaxCustom = mAxisRect->axis(QCPAxis::atBottom)->range().upper;
            }
        }

        mXAxisMinCustom = mAxisRect->axis(QCPAxis::atBottom)->range().lower;
        mXAxisMaxCustom = mAxisRect->axis(QCPAxis::atBottom)->range().upper;
        break;
    case asCustom:
        mAxisRect->axis(QCPAxis::atBottom)->setRange(QCPRange(mXAxisMinCustom, mXAxisMaxCustom));
        break;
    }
}

void PlotXY::updateItems()
{
    QList<QCPAbstractItem*> items = mAxisRect->items();
    for(int i=0; i<items.count();i++)
        mBoard->removeItem(items.at(i));

    mHLines.clear();
    mVLines.clear();
    mTracers.clear();

    int yParamSize = mYParameters.count();
    for(int i=0; i< yParamSize; i++) {
        QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
        if(dashParam) {

            QColor color = dashParam->parameterConfiguration()->defaultColorSettingsRef().color();
            QPen linePen(color);
            linePen.setStyle(Qt::DashLine);
            linePen.setWidthF(dashParam->parameterConfiguration()->penWidth());

            auto tracer = new QCPItemTracer(mBoard);
            tracer->position->setAxisRect(mAxisRect);
            tracer->position->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            tracer->setLayer(QLatin1String("axes"));
            tracer->position->setType(QCPItemPosition::ptPlotCoords);
            tracer->setClipAxisRect(mAxisRect);
            tracer->setClipToAxisRect(true);
            tracer->setStyle(QCPItemTracer::tsCircle);
            tracer->setPen(linePen);
            tracer->setBrush(color);
            tracer->setSize(8);
            tracer->setVisible(dashParam->parameterConfiguration()->curveTracerVisible());
            tracer->setInterpolating(false);
            mTracers.append(tracer);

            QCPItemStraightLine* vLine  = new QCPItemStraightLine(mBoard);
            vLine->setClipAxisRect(mAxisRect);
            vLine->setClipToAxisRect(true);
            vLine->setLayer(QLatin1String("axes"));
            vLine->point1->setAxisRect(mAxisRect);
            vLine->point2->setAxisRect(mAxisRect);
            vLine->point1->setType(QCPItemPosition::ptPlotCoords);
            vLine->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            vLine->point2->setType(QCPItemPosition::ptPlotCoords);
            vLine->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            vLine->setPen(linePen);
            vLine->setVisible(dashParam->parameterConfiguration()->curveTracerVisible());
            vLine->point1->setCoords(0,0);
            vLine->point2->setCoords(0,1);
            mVLines.append(vLine);

            QCPItemStraightLine* hLine  = new QCPItemStraightLine(mBoard);
            hLine->setClipAxisRect(mAxisRect);
            hLine->setClipToAxisRect(true);
            hLine->setLayer(QLatin1String("axes"));
            hLine->point1->setType(QCPItemPosition::ptPlotCoords);
            hLine->point2->setType(QCPItemPosition::ptPlotCoords);
            hLine->point1->setAxisRect(mAxisRect);
            hLine->point2->setAxisRect(mAxisRect);
            hLine->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            hLine->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            hLine->setPen(linePen);
            hLine->setVisible(dashParam->parameterConfiguration()->curveTracerVisible());
            mHLines.append(hLine);
        }
    }

    if(mYThresholdsVisible) {
        int yParamSize = mYParameters.count();
        for(int i=0; i< yParamSize; i++) {
            QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
            if(dashParam) {
                if(dashParam->parameterConfiguration()->itemsThresholdsVisible()) {
                    QMap<double, ColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
                    QMap<double, ColorSettings>::iterator lowIt;
                    for (lowIt = lowThr.begin();
                         lowIt != lowThr.end(); ++lowIt) {

                        auto l = new QCPItemStraightLine(mBoard);
                        l->setLayer(QLatin1String("grid"));
                        l->point1->setType(QCPItemPosition::ptPlotCoords);
                        l->point2->setType(QCPItemPosition::ptPlotCoords);
                        l->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                        l->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                        l->point1->setCoords(0.0, lowIt.key());
                        l->point2->setCoords(1.0, lowIt.key());
                        l->setClipAxisRect(mAxisRect);
                        l->setClipToAxisRect(true);
                        QColor color = lowIt.value().color();
                        color.setAlpha(150);
                        QPen pen = QPen(color, 0, Qt::DashDotDotLine);
                        l->setPen(pen);
                    }

                    QMap<double, ColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
                    QMap<double, ColorSettings>::iterator highIt;
                    for (highIt = highThr.begin();
                         highIt != highThr.end(); ++highIt) {

                        auto l = new QCPItemStraightLine(mBoard);
                        l->setLayer(QLatin1String("grid"));
                        l->point1->setType(QCPItemPosition::ptPlotCoords);
                        l->point2->setType(QCPItemPosition::ptPlotCoords);
                        l->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                        l->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                        l->point1->setCoords(0.0, highIt.key());
                        l->point2->setCoords(1.0, highIt.key());
                        l->setClipAxisRect(mAxisRect);
                        l->setClipToAxisRect(true);

                        QColor color = highIt.value().color();
                        color.setAlpha(150);
                        QPen pen = QPen(color, 0, Qt::DashDotDotLine);
                        l->setPen(pen);
                    }
                }
            }
        }
    }

    if(mXThresholdsVisible) {
        if(mXParameter) {
            if(mXParameter->parameterConfiguration()->itemsThresholdsVisible()) {
                QMap<double, ColorSettings> lowThr = mXParameter->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
                QMap<double, ColorSettings>::iterator lowIt;
                for (lowIt = lowThr.begin();
                     lowIt != lowThr.end(); ++lowIt) {

                    auto l = new QCPItemStraightLine(mBoard);
                    l->setLayer(QLatin1String("grid"));
                    l->point1->setType(QCPItemPosition::ptPlotCoords);
                    l->point2->setType(QCPItemPosition::ptPlotCoords);
                    l->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                    l->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                    l->point1->setCoords(lowIt.key(), 0.0);
                    l->point2->setCoords(lowIt.key(), 1.0);
                    l->setClipAxisRect(mAxisRect);
                    l->setClipToAxisRect(true);
                    QColor color = lowIt.value().color();
                    color.setAlpha(150);
                    QPen pen = QPen(color, 0, Qt::DashDotDotLine);

                    l->setPen(pen);
                }

                QMap<double, ColorSettings> highThr = mXParameter->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
                QMap<double, ColorSettings>::iterator highIt;
                for (highIt = highThr.begin();
                     highIt != highThr.end(); ++highIt) {

                    auto l = new QCPItemStraightLine(mBoard);
                    l->setLayer(QLatin1String("grid"));
                    l->point1->setType(QCPItemPosition::ptPlotCoords);
                    l->point2->setType(QCPItemPosition::ptPlotCoords);
                    l->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                    l->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
                    l->point1->setCoords(highIt.key(), 0.0);
                    l->point2->setCoords(highIt.key(), 1.0);
                    l->setClipAxisRect(mAxisRect);
                    l->setClipToAxisRect(true);
                    QColor color = highIt.value().color();
                    color.setAlpha(150);
                    QPen pen = QPen(color, 0, Qt::DashDotDotLine);
                    l->setPen(pen);
                }
            }
        }
    }

    for(auto plottable : mAxisRect->axis(QCPAxis::atRight)->plottables())
        mBoard->removePlottable(plottable);

    for(auto patron : mPatrons) {
        QCPCurve *curve = new QCPCurve(mAxisRect->axis(QCPAxis::atTop), mAxisRect->axis(QCPAxis::atRight));
        curve->setPen(patron->pen());

        QList<double> x = patron->xValues();
        QList<double> y = patron->yValues();
        if(x.count() == y.count()) {
            for(int i=0;i<x.count();i++)
                curve->addData(i, x.at(i), y.at(i));
        }
    }
}

void PlotXY::updateGraphsStyle()
{
    int yParamSize = mYParameters.count();
    for(int i=0; i< yParamSize; i++) {
        QSharedPointer<BoardParameter> dashParam = mYParameters.at(i);
        if(dashParam) {
            if(i < mCurves.count()) {
                mCurves.at(i)->setPen(QPen(dashParam->parameterConfiguration()->defaultColorSettingsRef().color(),dashParam->parameterConfiguration()->penWidth()));
                mCurves.at(i)->setLineStyle(dashParam->parameterConfiguration()->curveLineStyle());
                mCurves.at(i)->setScatterStyle(dashParam->parameterConfiguration()->scatterStyle());
            }
        }
    }
}

void PlotXY::addPatron(QSharedPointer<CurvePatron> patron)
{
    mPatrons.append(patron);
    updateItems();
}

void PlotXY::removePatron(int index)
{
    if(index < mPatrons.count())
        mPatrons.removeAt(index);
}

void PlotXY::update(QCPLayoutElement::UpdatePhase phase)
{
    BoardElement::update(phase);
    if(phase == upLayout) {
        mMainLayout->setOuterRect(rect());
        applySizeConstraints();
    }
    mMainLayout->update(phase);
}

QSharedPointer<BoardParameter> PlotXY::xParameter() const
{
    return mXParameter;
}

bool PlotXY::xThresholdsVisible() const
{
    return mXThresholdsVisible;
}

void PlotXY::setXThresholdsVisible(bool xThresholdsVisible)
{
    mXThresholdsVisible = xThresholdsVisible;
}

QList<QSharedPointer<CurvePatron> > PlotXY::patrons() const
{
    return mPatrons;
}

QList<QSharedPointer<BoardParameter> > PlotXY::yParameters() const
{
    return mYParameters;
}

bool PlotXY::plotInRangePoints() const
{
    return mPlotInRangePoints;
}

void PlotXY::setPlotInRangePoints(bool plotInRangePoints)
{
    mPlotInRangePoints = plotInRangePoints;
}

bool PlotXY::titleVisible() const
{
    return mTitleVisible;
}

void PlotXY::setTitleVisible(bool titleVisible)
{
    mTitleVisible = titleVisible;
}

QString PlotXY::title() const
{
    return mTitle;
}

void PlotXY::setTitle(const QString &title)
{
    mTitle = title;
}

int PlotXY::xAxisTicksCount() const
{
    return mXAxisTicksCount;
}

void PlotXY::setXAxisTicksCount(int xAxisTicksCount)
{
    mXAxisTicksCount = xAxisTicksCount;
}

int PlotXY::yAxisTicksCount() const
{
    return mYAxisTicksCount;
}

void PlotXY::setYAxisTicksCount(int yAxisTicksCount)
{
    mYAxisTicksCount = yAxisTicksCount;
}

bool PlotXY::yAxisLineVisible() const
{
    return mYAxisLineVisible;
}

void PlotXY::setYAxisLineVisible(bool yAxisLineVisible)
{
    mYAxisLineVisible = yAxisLineVisible;
}

bool PlotXY::xAxisLineVisible() const
{
    return mXAxisLineVisible;
}

void PlotXY::setXAxisLineVisible(bool xAxisLineVisible)
{
    mXAxisLineVisible = xAxisLineVisible;
}

bool PlotXY::yThresholdsVisible() const
{
    return mYThresholdsVisible;
}

void PlotXY::setYThresholdsVisible(bool yThresholdsVisible)
{
    mYThresholdsVisible = yThresholdsVisible;
}

double PlotXY::xAxisMinCustom() const
{
    return mXAxisMinCustom;
}

void PlotXY::setXAxisMinCustom(double xAxisMinCustom)
{
    mXAxisMinCustom = xAxisMinCustom;
}

double PlotXY::xAxisMaxCustom() const
{
    return mXAxisMaxCustom;
}

void PlotXY::setXAxisMaxCustom(double xAxisMaxCustom)
{
    mXAxisMaxCustom = xAxisMaxCustom;
}

double PlotXY::yAxisMinCustom() const
{
    return mYAxisMinCustom;
}

void PlotXY::setYAxisMinCustom(double yAxisMinCustom)
{
    mYAxisMinCustom = yAxisMinCustom;
}

double PlotXY::yAxisMaxCustom() const
{
    return mYAxisMaxCustom;
}

void PlotXY::setYAxisMaxCustom(double yAxisMaxCustom)
{
    mYAxisMaxCustom = yAxisMaxCustom;
}

int PlotXY::xAxisHistory() const
{
    return mXAxisHistory;
}

void PlotXY::setXAxisHistory(int xAxisHistory)
{
    mXAxisHistory = xAxisHistory;
}

bool PlotXY::xAxisGridVisible() const
{
    return mXAxisGridVisible;
}

void PlotXY::setXAxisGridVisible(bool xAxisGridVisible)
{
    mXAxisGridVisible = xAxisGridVisible;
}

bool PlotXY::xAxisLabelsVisible() const
{
    return mXAxisLabelsVisible;
}

void PlotXY::setXAxisLabelsVisible(bool xAxisLabelsVisible)
{
    mXAxisLabelsVisible = xAxisLabelsVisible;
}

bool PlotXY::xAxisTicksVisible() const
{
    return mXAxisTicksVisible;
}

void PlotXY::setXAxisTicksVisible(bool xAxisTicksVisible)
{
    mXAxisTicksVisible = xAxisTicksVisible;
}

bool PlotXY::yAxisGridVisible() const
{
    return mYAxisGridVisible;
}

void PlotXY::setYAxisGridVisible(bool yAxisGridVisible)
{
    mYAxisGridVisible = yAxisGridVisible;
}

bool PlotXY::yAxisLabelsVisible() const
{
    return mYAxisLabelsVisible;
}

void PlotXY::setYAxisLabelsVisible(bool yAxisLabelsVisible)
{
    mYAxisLabelsVisible = yAxisLabelsVisible;
}

bool PlotXY::yAxisTicksVisible() const
{
    return mYAxisTicksVisible;
}

void PlotXY::setYAxisTicksVisible(bool yAxisTicksVisible)
{
    mYAxisTicksVisible = yAxisTicksVisible;
}

PlotXY::YLegendPosition PlotXY::yLegendPosition() const
{
    return mYLegendPosition;
}

void PlotXY::setYLegendPosition(const YLegendPosition &yLegendPosition)
{
    mYLegendPosition = yLegendPosition;
}

PlotXY::XLegendPosition PlotXY::xLegendPosition() const
{
    return mXLegendPosition;
}

void PlotXY::setXLegendPosition(const XLegendPosition &xLegendPosition)
{
    mXLegendPosition = xLegendPosition;
}

PlotXY::AxisScale PlotXY::yAxisScale() const
{
    return mYAxisScale;
}

void PlotXY::setYAxisScale(const AxisScale &yAxisScale)
{
    mYAxisScale = yAxisScale;
}

PlotXY::AxisScale PlotXY::xAxisScale() const
{
    return mXAxisScale;
}

void PlotXY::setXAxisScale(const AxisScale &xAxisScale)
{
    mXAxisScale = xAxisScale;
}

bool PlotXY::yLegendVisible() const
{
    return mYLegendVisible;
}

void PlotXY::setYLegendVisible(bool yLegendVisible)
{
    mYLegendVisible = yLegendVisible;
}

bool PlotXY::xLegendVisible() const
{
    return mXLegendVisible;
}

void PlotXY::setXLegendVisible(bool xLegendVisible)
{
    mXLegendVisible = xLegendVisible;
}

int PlotXY::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int PlotXY::defaultHeight()
{
    return 3*mBoard->dashboardLayout()->singleElementRowCount();
}

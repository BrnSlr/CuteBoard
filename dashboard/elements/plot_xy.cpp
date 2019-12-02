#include "dashboard/elements/plot_xy.h"
#include <utility>
#include "dashboard/elements/value_display.h"
#include "ui/element_editors/curves_xy_editor.h"

QTBPlotXY::QTBPlotXY(QTBoard *dashboard) :
    QTBDashboardElement (dashboard),
    mLayout(new QCPLayoutGrid),
    mYLegendPosition(lpLeft),
    mYAxisScale(asAuto),
    mYLegendVisible(true),
    mXLegendPosition(lpBottom),
    mXAxisScale(asAuto),
    mXLegendVisible(true),
    mYAxisTicksVisible(false),
    mYAxisLabelsVisible(true),
    mYAxisGridVisible(true),
    mXAxisTicksVisible(false),
    mXAxisLabelsVisible(true),
    mXAxisGridVisible(true),
    mXAxisHistory(10),
    mYThresholdsVisible(true),
    mXThresholdsVisible(true),
    mXParameter(nullptr)
{
    setParametersMaxCount(9);
    setConfigurationMode(QTBParameterConfiguration::cmCurveX);
}

void QTBPlotXY::clearElement()
{
    if(mLayout) {
        for(int i=0;i<mYLegendLayout->elementCount();i++) {
            auto  element = qobject_cast<QTBValueDisplay *>(mYLegendLayout->elementAt(i));
            if(element)
                element->clearElement();
        }

        for(int i=0;i<mXLegendLayout->elementCount();i++) {
            auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(i));
            if(element)
                element->clearElement();
        }

        QList<QCPAbstractItem*> items = mAxisRect->items();
        for(int i=0; i<items.count();i++)
            mBoard->removeItem(items.at(i));

        mAxisRect->clearPlottables();
        mYLegendLayout->clear();
        mXLegendLayout->clear();
        mLayout->clear();
        delete mLayout;
    }
}

void QTBPlotXY::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {

        QTBDashboardElement::initializeElement(dashboard);

        mLayout->initializeParentPlot(dashboard);
        mLayout->setParent(this);

        mAxisRect = new QTBAxisRect(dashboard);
        connect(mAxisRect->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)),
                mAxisRect->axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));
        connect(mAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),
                mAxisRect->axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));
        mYLegendLayout = new QTBLayoutGrid();
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

        mXLegendLayout = new QTBLayoutGrid();
        mXLegendLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        mXLegendLayout->initializeParentPlot(dashboard);
        mXLegendLayout->setColumnSpacing(1);
        mXLegendLayout->setMinimumSize(50,50);

        mXLegendEmptyElementLeft = new QCPLayoutElement(dashboard);
        mXLegendEmptyElementLeft->setMinimumSize(0.1,0.1);
        mXLegendEmptyElementRight = new QCPLayoutElement(dashboard);
        mXLegendEmptyElementRight->setMinimumSize(0.1,0.1);
        mXLegendLayout->addElement(mXLegendEmptyElementLeft);
        mXLegendLayout->addElement(mXLegendEmptyElementRight);
        mXLegendLayout->setColumnStretchFactor(0,0.01);

        mLayout->addElement(mAxisRect);
    }
}

void QTBPlotXY::edit()
{
    QTBCurvesXYEditor editor(this);
    editor.exec();
}

void QTBPlotXY::loadSettings(QSettings *settings)
{
    settings->beginGroup("General");
    if(settings->contains("YLegendVisible"))
        setYLegendVisible(settings->value("YLegendVisible").toBool());
    if(settings->contains("YLegendPosition"))
        setYLegendPosition(QTBPlotXY::YLegendPosition(settings->value("YLegendPosition").toInt()));
    if(settings->contains("XLegendVisible"))
        setXLegendVisible(settings->value("XLegendVisible").toBool());
    if(settings->contains("XLegendPosition"))
        setXLegendPosition(QTBPlotXY::XLegendPosition(settings->value("XLegendPosition").toInt()));
    if(settings->contains("TransparentBackground"))
        setTransparentBackground(settings->value("TransparentBackground").toBool());
    settings->endGroup();

    settings->beginGroup("YAxis");
    if(settings->contains("TicksVisible"))
        setYAxisTicksVisible(settings->value("TicksVisible").toBool());
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
        QSharedPointer<QTBCurvePatronConfiguration> patron(new QTBCurvePatronConfiguration());
        patron->load(settings);
        mPatrons.append(patron);
    }
    settings->endArray();

}

void QTBPlotXY::loadConfigurations(QSettings *settings)
{
    mXParameter = nullptr;
    if(settings->childGroups().contains("XParameter")) {
        settings->beginGroup("XParameter");

        mXParameter =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(mBoard));
        mXParameter->loadParameterSettings(settings, QTBParameterConfiguration::cmCurveX);

        auto legend = new QTBValueDisplay(mBoard);
        legend->initializeElement(mBoard);
        legend->addParameter(mXParameter);
        legend->setTransparentBackground(true);
        legend->setOrientation(QTBValueDisplay::doVerticalAlignCenter);

        mXLegendLayout->take(mXLegendEmptyElementRight);
        mXLegendLayout->simplify();
        mXLegendLayout->addElement(legend);
        mXLegendLayout->addElement(mXLegendEmptyElementRight);
        mXLegendLayout->setColumnStretchFactor(mXLegendLayout->columnCount()-1 , 0.01);

        settings->endGroup();
    }

    int paramCount = settings->beginReadArray("Parameters");

    for(int i= 0; i< paramCount;i++) {
        settings->setArrayIndex(i);
        QSharedPointer<QTBDashboardParameter> dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(mBoard));
        mDashParameters.append(dashParam);
        dashParam->loadParameterSettings(settings, QTBParameterConfiguration::cmCurveY);

        auto legend = new QTBValueDisplay(mBoard);
        legend->initializeElement(mBoard);
        legend->addParameter(dashParam);
        legend->setTransparentBackground(true);
        legend->setOrientation(QTBValueDisplay::doVerticalAlignLeftRight);

        mYLegendLayout->take(mYLegendEmptyElementBottom);
        mYLegendLayout->simplify();
        mYLegendLayout->addElement(legend);
        mYLegendLayout->addElement(mYLegendEmptyElementBottom);
        mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);

        auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
        mCurves.append(curve);
    }
    settings->endArray();

    updateElement();
}

void QTBPlotXY::checkParameters()
{
    QTBDashboardElement::checkParameters();

    QSharedPointer<QTBParameter> param;
    if(mBoard->dataManager()) {
        if(mXParameter) {
            param = mBoard->dataManager()->parameter(mXParameter->getLabel());
            if(param){
                mXParameter->setUnit(param->unit());
                mXParameter->setParameterId(param->parameterId());
            }
        }
    }
}

void QTBPlotXY::saveSettings(QSettings *settings)
{
    settings->beginGroup("General");
    settings->setValue("YLegendVisible", mYLegendVisible);
    settings->setValue("YLegendPosition", mYLegendPosition);
    settings->setValue("XLegendVisible", mXLegendVisible);
    settings->setValue("XLegendPosition", mXLegendPosition);
    settings->setValue("TransparentBackground", mTransparentBackground);
    settings->endGroup();

    settings->beginGroup("YAxis");
    settings->setValue("TicksVisible", mYAxisTicksVisible);
    settings->setValue("LabelsVisible", mYAxisLabelsVisible);
    settings->setValue("GridVisible", mYAxisGridVisible);
    settings->setValue("ScaleMode", mYAxisScale);
    settings->setValue("ScaleMin", mYAxisMinCustom);
    settings->setValue("ScaleMax", mYAxisMaxCustom);
    settings->setValue("ThresholdsVisible", mYThresholdsVisible);
    settings->endGroup();

    settings->beginGroup("XAxis");
    settings->setValue("TicksVisible", mXAxisTicksVisible);
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

void QTBPlotXY::saveConfigurations(QSettings *settings)
{
    settings->beginWriteArray("Parameters");
    int count = -1;
    for(int i= 0; i< mDashParameters.count();i++) {
        if(mDashParameters.at(i) != mXParameter) {
            count ++;
            settings->setArrayIndex(count);
            mDashParameters.at(i)->saveParameterSettings(settings, QTBParameterConfiguration::cmCurveY);
        }
    }
    settings->endArray();

    if(mXParameter) {
        settings->beginGroup("XParameter");
        mXParameter->saveParameterSettings(settings, QTBParameterConfiguration::cmCurveX);
        settings->endGroup();
    }
}

void QTBPlotXY::droppedParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
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

void QTBPlotXY::droppedParameter(QSharedPointer<QTBParameter> dataParameter)
{
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

QSharedPointer<QTBDashboardParameter> QTBPlotXY::addYParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    QSharedPointer<QTBDashboardParameter> dashParam = QTBDashboardElement::addParameter(std::move(parameterSettings));

    auto legend = new QTBValueDisplay(mBoard);
    legend->initializeElement(mBoard);
    legend->addParameter(dashParam);
    legend->setTransparentBackground(true);
    legend->setOrientation(QTBValueDisplay::doVerticalAlignLeftRight);

    mYLegendLayout->take(mYLegendEmptyElementBottom);
    mYLegendLayout->simplify();
    mYLegendLayout->addElement(legend);
    mYLegendLayout->addElement(mYLegendEmptyElementBottom);
    mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);
    mYLegendLayout->needUpdate(true);

    auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
    mCurves.append(curve);

    updateElement();

    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBPlotXY::addYParameter(QString paramLabel)
{
    QSharedPointer<QTBDashboardParameter> dashParam = QTBDashboardElement::addParameter(std::move(paramLabel));

    auto legend = new QTBValueDisplay(mBoard);
    legend->initializeElement(mBoard);
    legend->addParameter(dashParam);
    legend->setTransparentBackground(true);
    legend->setOrientation(QTBValueDisplay::doVerticalAlignLeftRight);

    mYLegendLayout->take(mYLegendEmptyElementBottom);
    mYLegendLayout->simplify();
    mYLegendLayout->addElement(legend);
    mYLegendLayout->addElement(mYLegendEmptyElementBottom);
    mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);
    mYLegendLayout->needUpdate(true);

    auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
    mCurves.append(curve);

    updateElement();

    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBPlotXY::addYParameter(QSharedPointer<QTBParameter> dataParameter)
{
    QSharedPointer<QTBDashboardParameter> dashParam = QTBDashboardElement::addParameter(std::move(dataParameter));

    auto legend = new QTBValueDisplay(mBoard);
    legend->initializeElement(mBoard);
    legend->addParameter(dashParam);
    legend->setTransparentBackground(true);
    legend->setOrientation(QTBValueDisplay::doVerticalAlignLeftRight);

    mYLegendLayout->take(mYLegendEmptyElementBottom);
    mYLegendLayout->simplify();
    mYLegendLayout->addElement(legend);
    mYLegendLayout->addElement(mYLegendEmptyElementBottom);
    mYLegendLayout->setRowStretchFactor(mYLegendLayout->rowCount()-1 , 0.01);
    mYLegendLayout->needUpdate(true);

    auto curve = new QCPCurve(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
    mCurves.append(curve);

    updateElement();

    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBPlotXY::addXParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(1));
    if(element) {
        element->clearElement();
        mXLegendLayout->remove(element);
    }

    if(mXParameter) {
        mXParameter.reset();
        mXParameter = nullptr;
    }
    mXParameter = QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(std::move(parameterSettings), mBoard));

    auto legend = new QTBValueDisplay(mBoard);
    legend->initializeElement(mBoard);
    legend->addParameter(mXParameter);
    legend->setTransparentBackground(true);
    legend->setOrientation(QTBValueDisplay::doVerticalAlignCenter);

    mXLegendLayout->take(mXLegendEmptyElementRight);
    mXLegendLayout->simplify();
    mXLegendLayout->addElement(legend);
    mXLegendLayout->addElement(mXLegendEmptyElementRight);
    mXLegendLayout->setColumnStretchFactor(mXLegendLayout->columnCount()-1 , 0.01);
    mXLegendLayout->needUpdate(true);

    updateElement();

    for(int i=0; i< mCurves.count();i++)
        mCurves.at(i)->data()->clear();

    return mXParameter;
}

QSharedPointer<QTBDashboardParameter> QTBPlotXY::addXParameter(QString paramLabel)
{
    auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(1));
    if(element) {
        element->clearElement();
        mXLegendLayout->remove(element);
    }

    if(mXParameter) {
        mXParameter.reset();
        mXParameter = nullptr;
    }
    mXParameter = QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(paramLabel, mBoard));

    auto legend = new QTBValueDisplay(mBoard);
    legend->initializeElement(mBoard);
    legend->addParameter(mXParameter);
    legend->setTransparentBackground(true);
    legend->setOrientation(QTBValueDisplay::doVerticalAlignCenter);

    mXLegendLayout->take(mXLegendEmptyElementRight);
    mXLegendLayout->simplify();
    mXLegendLayout->addElement(legend);
    mXLegendLayout->addElement(mXLegendEmptyElementRight);
    mXLegendLayout->setColumnStretchFactor(mXLegendLayout->columnCount()-1 , 0.01);
    mXLegendLayout->needUpdate(true);

    updateElement();

    for(int i=0; i< mCurves.count();i++)
        mCurves.at(i)->data()->clear();

    return mXParameter;
}

QSharedPointer<QTBDashboardParameter> QTBPlotXY::addXParameter(const QSharedPointer<QTBParameter>& dataParameter)
{
    auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(1));
    if(element) {
        element->clearElement();
        mXLegendLayout->remove(element);
    }

    if(mXParameter) {
        mXParameter.reset();
        mXParameter = nullptr;
    }
    mXParameter = QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(dataParameter, mBoard));

    auto legend = new QTBValueDisplay(mBoard);
    legend->initializeElement(mBoard);
    legend->addParameter(mXParameter);
    legend->setTransparentBackground(true);
    legend->setOrientation(QTBValueDisplay::doVerticalAlignCenter);

    mXLegendLayout->take(mXLegendEmptyElementRight);
    mXLegendLayout->simplify();
    mXLegendLayout->addElement(legend);
    mXLegendLayout->addElement(mXLegendEmptyElementRight);
    mXLegendLayout->setColumnStretchFactor(mXLegendLayout->columnCount()-1 , 0.01);
    mXLegendLayout->needUpdate(true);

    updateElement();

    for(int i=0; i< mCurves.count();i++)
        mCurves.at(i)->data()->clear();

    return mXParameter;
}

void QTBPlotXY::removeYParameter(int index)
{
    QCPCurve *curve = mCurves.takeAt(index);
    mBoard->removePlottable(curve);

    QCPItemStraightLine *vLine = mVLines.takeAt(index);
    mBoard->removeItem(vLine);

    QCPItemStraightLine *hLine = mHLines.takeAt(index);
    mBoard->removeItem(hLine);

    QCPLayoutElement *el = mYLegendLayout->takeAt(index + 1);
    mYLegendLayout->simplify();
    mYLegendLayout->needUpdate(true);
    auto  element = qobject_cast<QTBValueDisplay *>(el);
    if(element)
        element->clearElement();
    el->deleteLater();
    QTBDashboardElement::removeDashParameter(index);

    updateElement();
}

void QTBPlotXY::removeXParameter()
{
    QCPLayoutElement *el = mXLegendLayout->takeAt(1);
    mXLegendLayout->simplify();
    mXLegendLayout->needUpdate(true);
    auto  element = qobject_cast<QTBValueDisplay *>(el);
    if(element)
        element->clearElement();
    el->deleteLater();

    mXParameter.reset();

    updateElement();
}

void QTBPlotXY::updateElement()
{
    updateLegendSize();
    updateLayout();

    for(int i=0;i<mYLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<QTBValueDisplay *>(mYLegendLayout->elementAt(i));
        if(element) {
            element->updateElement();
        }
    }

    for(int i=0;i<mXLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(i));
        if(element) {
            element->updateElement();
        }
    }

    updateItems();
    updateGraphsStyle();
}

void QTBPlotXY::processNewSamples()
{
    for(int i=0;i<mYLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<QTBValueDisplay *>(mYLegendLayout->elementAt(i));
        if(element) {
            element->processNewSamples();
        }
    }

    for(int i=0;i<mXLegendLayout->elementCount();i++) {
        auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(i));
        if(element) {
            element->processNewSamples();
        }
    }

    if(mXParameter)
    {
        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(i);
            if(dashParam && dashParam->getParameterId() > 0) {
                if(i < mCurves.count()) {
                    mCurves.at(i)->addData(mXParameter->getTimestamp(), mXParameter->getValueDouble(), dashParam->getValueDouble() );
                    mCurves.at(i)->data()->removeBefore(mXParameter->getTimestamp()-mXAxisHistory);

                    mHLines.at(i)->point1->setCoords(0, dashParam->getValueDouble());
                    mHLines.at(i)->point2->setCoords(1, dashParam->getValueDouble());

                    mVLines.at(i)->point1->setCoords(mXParameter->getValueDouble(), 0);
                    mVLines.at(i)->point2->setCoords(mXParameter->getValueDouble(), 1);
                }
            }
        }
    }
}

void QTBPlotXY::updateLegendSize()
{
    QSizeF size = mBoard->dashboardLayout()->singleElementSize();

    if(mYLegendVisible) {
    mYLegendLayout->setMinimumSize(size.width(), 50);
    for(int i=1; i< mYLegendLayout->elementCount() - 1;i++) {
        if( qobject_cast<QTBValueDisplay *>(mYLegendLayout->elementAt(i))) {
            mYLegendLayout->elementAt(i)->setMinimumSize(size.width(), 0);
            mYLegendLayout->elementAt(i)->setMaximumSize(size.width(), size.height());
        }
    }
    }

    if(mXLegendVisible) {
    mXLegendLayout->setMinimumSize(50, size.height());
    auto  element = qobject_cast<QTBValueDisplay *>(mXLegendLayout->elementAt(1));
    if(element) {
        element->setMinimumSize(0, size.height());
        element->setMaximumSize(size.width(), size.height());
    }
}
}

void QTBPlotXY::updateAxes()
{
    switch(mYAxisScale) {
    case asAuto:
    {
        bool canRescale = false;
        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(i);
            if(dashParam) {
                if(dashParam->getParameterId() > 0) {
                    canRescale = true;
                    break;
                }
            }
        }
        if(canRescale) {
            if(mAxisRect->plottables().count() > 0) {
                mAxisRect->axis(QCPAxis::atLeft)->rescale();
                mAxisRect->axis(QCPAxis::atLeft)->scaleRange(1.05, mAxisRect->axis(QCPAxis::atLeft)->range().center());
                mYAxisMinCustom = mAxisRect->axis(QCPAxis::atLeft)->range().lower;
                mYAxisMaxCustom = mAxisRect->axis(QCPAxis::atLeft)->range().upper;
            }
        }
    }
        break;
    case asParam:
    {
        bool validRange = false;
        bool canAutoRescale = false;
        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(i);
            if(dashParam) {
                if(dashParam->getParameterId() > 0)
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
                mAxisRect->axis(QCPAxis::atLeft)->rescale();
                mAxisRect->axis(QCPAxis::atLeft)->scaleRange(1.05, mAxisRect->axis(QCPAxis::atLeft)->range().center());
                mYAxisMinCustom = mAxisRect->axis(QCPAxis::atLeft)->range().lower;
                mYAxisMaxCustom = mAxisRect->axis(QCPAxis::atLeft)->range().upper;
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
            if(mXParameter->getParameterId() > 0) {
                mAxisRect->axis(QCPAxis::atBottom)->rescale();
                mAxisRect->axis(QCPAxis::atBottom)->scaleRange(1.05, mAxisRect->axis(QCPAxis::atBottom)->range().center());
                mXAxisMinCustom = mAxisRect->axis(QCPAxis::atBottom)->range().lower;
                mXAxisMaxCustom = mAxisRect->axis(QCPAxis::atBottom)->range().upper;
            }
        }
        break;
    case asParam:
        if(mXParameter) {
            if(mXParameter->parameterConfiguration()->validRange()) {
                mAxisRect->axis(QCPAxis::atBottom)->setRange(QCPRange(mXParameter->parameterConfiguration()->rangeMinimum(), mXParameter->parameterConfiguration()->rangeMaximum()));
            } else if(mXParameter->getParameterId() > 0) {
                mAxisRect->axis(QCPAxis::atBottom)->rescale();
                mAxisRect->axis(QCPAxis::atBottom)->scaleRange(1.05, mAxisRect->axis(QCPAxis::atBottom)->range().center());
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

void QTBPlotXY::updateItems()
{
    QList<QCPAbstractItem*> items = mAxisRect->items();
    for(int i=0; i<items.count();i++)
        mBoard->removeItem(items.at(i));

    mHLines.clear();
    mVLines.clear();

    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(i);
        if(dashParam) {

            QColor color = dashParam->parameterConfiguration()->defaultColorSettingsRef().color();
            QPen linePen(color);
            linePen.setStyle(Qt::DashLine);
            linePen.setWidth(0);

            QCPItemStraightLine* vLine  = new QCPItemStraightLine(mBoard);
            vLine->setClipAxisRect(mAxisRect);
            vLine->setClipToAxisRect(true);
            vLine->setLayer(QLatin1String("axes"));
            vLine->point1->setType(QCPItemPosition::ptPlotCoords);
            vLine->point2->setType(QCPItemPosition::ptPlotCoords);
            vLine->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            vLine->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            vLine->setPen(linePen);
            vLine->setVisible(dashParam->parameterConfiguration()->curveTracerVisible());
            mVLines.append(vLine);

            QCPItemStraightLine* hLine  = new QCPItemStraightLine(mBoard);
            hLine->setClipAxisRect(mAxisRect);
            hLine->setClipToAxisRect(true);
            hLine->setLayer(QLatin1String("axes"));
            hLine->point1->setType(QCPItemPosition::ptPlotCoords);
            hLine->point2->setType(QCPItemPosition::ptPlotCoords);
            hLine->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            hLine->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            hLine->setPen(linePen);
            hLine->setVisible(dashParam->parameterConfiguration()->curveTracerVisible());
            mHLines.append(hLine);
        }
    }

    if(mYThresholdsVisible) {
        for(int i=0; i< parametersCount(); i++) {
            QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(i);
            if(dashParam) {
                if(dashParam->parameterConfiguration()->itemsThresholdsVisible()) {
                    QMap<double, QTBColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
                    QMap<double, QTBColorSettings>::iterator lowIt;
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
                        QPen pen = QPen(color);
                        pen.setStyle(Qt::DashDotDotLine);
                        l->setPen(pen);
                    }

                    QMap<double, QTBColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
                    QMap<double, QTBColorSettings>::iterator highIt;
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
                        QPen pen = QPen(color);
                        pen.setStyle(Qt::DashDotDotLine);
                        l->setPen(pen);
                    }
                }
            }
        }
    }

    if(mXThresholdsVisible) {
        if(mXParameter) {
            if(mXParameter->parameterConfiguration()->itemsThresholdsVisible()) {
                QMap<double, QTBColorSettings> lowThr = mXParameter->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
                QMap<double, QTBColorSettings>::iterator lowIt;
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
                    QPen pen = QPen(color);
                    pen.setStyle(Qt::DashDotDotLine);

                    l->setPen(pen);
                }

                QMap<double, QTBColorSettings> highThr = mXParameter->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
                QMap<double, QTBColorSettings>::iterator highIt;
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
                    QPen pen = QPen(color);
                    pen.setStyle(Qt::DashDotDotLine);
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

void QTBPlotXY::updateGraphsStyle()
{
    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(i);
        if(dashParam) {
            if(i < mCurves.count()) {
                mCurves.at(i)->setPen(dashParam->parameterConfiguration()->defaultColorSettingsRef().color());
                mCurves.at(i)->setLineStyle(dashParam->parameterConfiguration()->curveLineStyle());
                mCurves.at(i)->setScatterStyle(dashParam->parameterConfiguration()->scatterStyle());
            }
        }
    }
}

void QTBPlotXY::updateLayout()
{
    mLayout->take(mAxisRect);
    mLayout->take(mYLegendLayout);
    mLayout->take(mXLegendLayout);
    mXLegendLayout->setVisible(false);
    mYLegendLayout->setVisible(false);
    mLayout->simplify();

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
                mLayout->take(mYLegendLayout);
                mYLegendLayout->setVisible(false);
            }

            if(mXLegendLayout->elementCount() > 2 && mXLegendVisible) {
                mLayout->addElement(1,0, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mLayout->take(mXLegendLayout);
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
                mLayout->take(mYLegendLayout);
                mYLegendLayout->setVisible(false);
            }

            mLayout->addElement(0,1, mAxisRect);

            if(mXLegendLayout->elementCount() > 2 && mXLegendVisible) {
                mLayout->addElement(1,1, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mLayout->take(mXLegendLayout);
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
            if(mXLegendLayout->elementCount() > 2 && mXLegendVisible) {
                mLayout->addElement(0,0, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mLayout->take(mXLegendLayout);
                mXLegendLayout->setVisible(false);
            }

            mLayout->addElement(1,0, mAxisRect);

            if(mYLegendLayout->elementCount() > 2 && mYLegendVisible) {
                mLayout->addElement(1,1, mYLegendLayout);
                mYLegendLayout->setVisible(true);
            } else {
                mLayout->take(mYLegendLayout);
                mYLegendLayout->setVisible(false);
            }

            mLayout->setColumnStretchFactor(1,0.01);
            mLayout->setColumnStretchFactor(0,1);
            mLayout->setRowStretchFactor(1,1);
            mLayout->setRowStretchFactor(0,0.01);
            break;
        case lpLeft:
            if(mXLegendLayout->elementCount() > 2 && mXLegendVisible) {
                mLayout->addElement(0,1, mXLegendLayout);
                mXLegendLayout->setVisible(true);
            } else {
                mLayout->take(mXLegendLayout);
                mXLegendLayout->setVisible(false);
            }

            if(mYLegendLayout->elementCount() > 2 && mYLegendVisible) {
                mLayout->addElement(1,0, mYLegendLayout);
                mYLegendLayout->setVisible(true);
            } else {
                mLayout->take(mYLegendLayout);
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
}

void QTBPlotXY::addPatron(QSharedPointer<QTBCurvePatronConfiguration> patron)
{
    mPatrons.append(patron);
    updateItems();
}

void QTBPlotXY::removePatron(int index)
{
    if(index < mPatrons.count())
        mPatrons.removeAt(index);
}

void QTBPlotXY::updateDashboardParameters(QTBDashboardParameter::UpdateMode mode)
{
    QTBDashboardElement::updateDashboardParameters(mode);

    if(mXParameter)
        mXParameter->update(mode);
}

void QTBPlotXY::update(QCPLayoutElement::UpdatePhase phase)
{
    QTBDashboardElement::update(phase);

    switch (phase)
    {
    case upPreparation:
    {
        updateAxes();
        updateLegendSize();
        break;
    }
    case upLayout:
    {
        mLayout->setOuterRect(rect());
        break;
    }
    default: break;
    }

    mLayout->update(phase);
}

QSharedPointer<QTBDashboardParameter> QTBPlotXY::xParameter() const
{
    return mXParameter;
}

bool QTBPlotXY::xThresholdsVisible() const
{
    return mXThresholdsVisible;
}

void QTBPlotXY::setXThresholdsVisible(bool xThresholdsVisible)
{
    if(mXThresholdsVisible != xThresholdsVisible) {
    mXThresholdsVisible = xThresholdsVisible;
}
}

QList<QSharedPointer<QTBCurvePatronConfiguration> > QTBPlotXY::patrons() const
{
    return mPatrons;
}

bool QTBPlotXY::yThresholdsVisible() const
{
    return mYThresholdsVisible;
}

void QTBPlotXY::setYThresholdsVisible(bool yThresholdsVisible)
{
    if(mYThresholdsVisible != yThresholdsVisible) {
    mYThresholdsVisible = yThresholdsVisible;
}
}

double QTBPlotXY::xAxisMinCustom() const
{
    return mXAxisMinCustom;
}

void QTBPlotXY::setXAxisMinCustom(double xAxisMinCustom)
{
    if(mXAxisMinCustom != xAxisMinCustom) {
    mXAxisMinCustom = xAxisMinCustom;
}
}

double QTBPlotXY::xAxisMaxCustom() const
{
    return mXAxisMaxCustom;
}

void QTBPlotXY::setXAxisMaxCustom(double xAxisMaxCustom)
{
    if(mXAxisMaxCustom != xAxisMaxCustom) {
    mXAxisMaxCustom = xAxisMaxCustom;
}
}

double QTBPlotXY::yAxisMinCustom() const
{
    return mYAxisMinCustom;
}

void QTBPlotXY::setYAxisMinCustom(double yAxisMinCustom)
{
    if(mYAxisMinCustom != yAxisMinCustom) {
    mYAxisMinCustom = yAxisMinCustom;
}
}

double QTBPlotXY::yAxisMaxCustom() const
{
    return mYAxisMaxCustom;
}

void QTBPlotXY::setYAxisMaxCustom(double yAxisMaxCustom)
{
    if(mYAxisMaxCustom != yAxisMaxCustom) {
    mYAxisMaxCustom = yAxisMaxCustom;
}
}

int QTBPlotXY::xAxisHistory() const
{
    return mXAxisHistory;
}

void QTBPlotXY::setXAxisHistory(int xAxisHistory)
{
    if(mXAxisHistory != xAxisHistory) {
    mXAxisHistory = xAxisHistory;
}
}

bool QTBPlotXY::xAxisGridVisible() const
{
    return mXAxisGridVisible;
}

void QTBPlotXY::setXAxisGridVisible(bool xAxisGridVisible)
{
    if(mXAxisGridVisible != xAxisGridVisible) {
    mXAxisGridVisible = xAxisGridVisible;
    mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(mXAxisGridVisible);
}
}

bool QTBPlotXY::xAxisLabelsVisible() const
{
    return mXAxisLabelsVisible;
}

void QTBPlotXY::setXAxisLabelsVisible(bool xAxisLabelsVisible)
{
    if(mXAxisLabelsVisible != xAxisLabelsVisible) {
    mXAxisLabelsVisible = xAxisLabelsVisible;
    mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(mXAxisLabelsVisible);
}
}

bool QTBPlotXY::xAxisTicksVisible() const
{
    return mXAxisTicksVisible;
}

void QTBPlotXY::setXAxisTicksVisible(bool xAxisTicksVisible)
{
    if(mXAxisTicksVisible != xAxisTicksVisible) {
    mXAxisTicksVisible = xAxisTicksVisible;
    if(mXAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(QPen(mBoard->frontColor()));
    } else {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);
    }
}
}

bool QTBPlotXY::yAxisGridVisible() const
{
    return mYAxisGridVisible;
}

void QTBPlotXY::setYAxisGridVisible(bool yAxisGridVisible)
{
    if(mYAxisGridVisible != yAxisGridVisible) {
    mYAxisGridVisible = yAxisGridVisible;
    mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(mYAxisGridVisible);
        mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(mYAxisGridVisible);
    }
}

bool QTBPlotXY::yAxisLabelsVisible() const
{
    return mYAxisLabelsVisible;
}

void QTBPlotXY::setYAxisLabelsVisible(bool yAxisLabelsVisible)
{
    if(yAxisLabelsVisible != mYAxisLabelsVisible) {
    mYAxisLabelsVisible = yAxisLabelsVisible;
    mAxisRect->axis(QCPAxis::atLeft)->setTickLabels(mYAxisLabelsVisible);
}
}

bool QTBPlotXY::yAxisTicksVisible() const
{
    return mYAxisTicksVisible;
}

void QTBPlotXY::setYAxisTicksVisible(bool yAxisTicksVisible)
{
    if(yAxisTicksVisible != mYAxisTicksVisible) {
    mYAxisTicksVisible = yAxisTicksVisible;
    if(mYAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(QPen(mBoard->frontColor()));
    } else {
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(Qt::NoPen);
    }
}
}

QTBPlotXY::YLegendPosition QTBPlotXY::yLegendPosition() const
{
    return mYLegendPosition;
}

void QTBPlotXY::setYLegendPosition(const YLegendPosition &yLegendPosition)
{
    if(yLegendPosition != mYLegendPosition) {
    mYLegendPosition = yLegendPosition;
    }
}

QTBPlotXY::XLegendPosition QTBPlotXY::xLegendPosition() const
{
    return mXLegendPosition;
}

void QTBPlotXY::setXLegendPosition(const XLegendPosition &xLegendPosition)
{
    if(xLegendPosition != mXLegendPosition) {
    mXLegendPosition = xLegendPosition;
    }
}

QTBPlotXY::AxisScale QTBPlotXY::yAxisScale() const
{
    return mYAxisScale;
}

void QTBPlotXY::setYAxisScale(const AxisScale &yAxisScale)
{
    if(yAxisScale != mYAxisScale) {
    mYAxisScale = yAxisScale;
}
}

QTBPlotXY::AxisScale QTBPlotXY::xAxisScale() const
{
    return mXAxisScale;
}

void QTBPlotXY::setXAxisScale(const AxisScale &xAxisScale)
{
    if(xAxisScale != mXAxisScale) {
    mXAxisScale = xAxisScale;
}
}

bool QTBPlotXY::yLegendVisible() const
{
    return mYLegendVisible;
}

void QTBPlotXY::setYLegendVisible(bool yLegendVisible)
{
    if(yLegendVisible != mYLegendVisible) {
    mYLegendVisible = yLegendVisible;
    }
}

bool QTBPlotXY::xLegendVisible() const
{
    return mXLegendVisible;
}

void QTBPlotXY::setXLegendVisible(bool xLegendVisible)
{
    if(xLegendVisible != mXLegendVisible) {
    mXLegendVisible = xLegendVisible;
    }
}

int QTBPlotXY::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBPlotXY::defaultHeight()
{
    return 3*mBoard->dashboardLayout()->singleElementRowCount();
}

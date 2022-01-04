#include "gauge_vertical.h"
#include "gauge_vertical_editor.h"
#include "ui/board/management/project.h"

GaugeVertical::GaugeVertical(Board *dashboard) :
    NumericalDisplay (dashboard),
    mValueAxisRange(QCPRange(0,0)),
    mAxisTicksVisible(true),
    mAxisLabelsVisible(true),
    mAxisLineVisible(false),
    mAxisGridVisible(false),
    mAxisScale(asAuto),
    mTankGauge(true),
    mGaugeVPosition(gpMiddle)
{

}

void GaugeVertical::initialize(Board *dashboard)
{
    if(dashboard) {
        NumericalDisplay::initialize(dashboard);

        auto textColor = QApplication::palette().toolTipText().color();
        mAxisRectThresholds = new AxisRect(dashboard);
        mAxisRectThresholds->axis(QCPAxis::atRight)->setVisible(false);
        mAxisRectThresholds->axis(QCPAxis::atBottom)->setVisible(false);
        mAxisRectThresholds->setMinimumMargins(QMargins(2, 10, 2, 10));
        mAxisRectThresholds->setMaximumSize(5,QWIDGETSIZE_MAX);
        mAxisRectThresholds->axis(QCPAxis::atBottom)->setRange(0, 1);

        mAxisRect = new AxisRect(dashboard);
        mAxisRect->setAxisColor(textColor);

        mAxisRect->setRangeZoomAxes(QList<QCPAxis*>());
        mAxisRect->setRangeDragAxes(QList<QCPAxis*>());

        mAxisRect->axis(QCPAxis::atRight)->setVisible(true);
        mAxisRect->axis(QCPAxis::atBottom)->setVisible(false);

        mAxisRect->axis(QCPAxis::atRight)->setBasePen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atRight)->setTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atRight)->setSubTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atRight)->setTickLabelColor(textColor);

        mAxisRect->axis(QCPAxis::atRight)->setTickLabels(mAxisTicksVisible);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(false);

        mAxisRect->axis(QCPAxis::atRight)->grid()->setVisible(mAxisGridVisible);
        mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(false);

        mAxisRect->axis(QCPAxis::atRight)->setTickLengthOut(mAxisRect->axis(QCPAxis::atRight)->tickLengthIn());
        mAxisRect->axis(QCPAxis::atRight)->setTickLengthIn(0);

        mAxisRect->axis(QCPAxis::atRight)->setSubTickLengthOut(mAxisRect->axis(QCPAxis::atRight)->subTickLengthIn());
        mAxisRect->axis(QCPAxis::atRight)->setSubTickLengthIn(0);

        mAxisRect->axis(QCPAxis::atBottom)->setRange(0.5, 1.5);
        mAxisRect->setMinimumMargins(QMargins(5, 10, 5, 10));

        mAxisRect->axis(QCPAxis::atRight)->setOffset(2);
        mAxisRect->setMaximumSize(300,QWIDGETSIZE_MAX);

        mBar = new QCPBars(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
        mBar->setWidth(1);
        mBar->setAntialiased(false);
        mBar->setAntialiasedFill(false);
        mBar->setPen(Qt::NoPen);
        mBar->setBrush(QColor(0,131,179,150));

        mValueAxisRange = QCPRange(0,0);

        mBarLayout = new LayoutGrid();
        mBarLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
        mBarLayout->initializeParentPlot(dashboard);
        mBarLayout->setColumnSpacing(2);

        mBarLayoutEmptyElementFirst = new QCPLayoutElement(dashboard);
        mBarLayoutEmptyElementFirst->setMinimumSize(0.1,0.1);
        mBarLayoutEmptyElementSecond = new QCPLayoutElement(dashboard);
        mBarLayoutEmptyElementSecond->setMinimumSize(0.1,0.1);

        mBarLayout->addElement(mBarLayoutEmptyElementFirst);
        mBarLayout->addElement(mAxisRectThresholds);
        mBarLayout->addElement(mAxisRect);
        mBarLayout->addElement(mBarLayoutEmptyElementSecond);

        mBarLayout->setColumnStretchFactor(0,0.01);
        mBarLayout->setColumnStretchFactor(3,0.01);
    }
}


void GaugeVertical::clearElement()
{
    for(auto tracer : mThresholdTracers)
        mBoard->removeItem(tracer);
    mThresholdTracers.clear();

    if(mAxisRect) {
        mAxisRect->clearPlottables();
        delete mAxisRect;
    }

    if(mAxisRectThresholds) {
        mAxisRectThresholds->clearPlottables();
        delete mAxisRectThresholds;
    }

    if(mBarLayout)
        mBarLayout->clear();

    NumericalDisplay::clearElement();
}

void GaugeVertical::edit()
{
    GaugeVerticalEditor editor(this);
    editor.exec();
}

void GaugeVertical::loadSettings(QSettings *settings)
{
    SingleDisplay::loadSettings(settings);

    settings->beginGroup("Gauge");
    if(settings->contains("TankMode"))
        setTankGauge(settings->value("TankMode").toBool());
    settings->endGroup();

    settings->beginGroup("Axis");
    if(settings->contains("TicksVisible"))
        setAxisTicksVisible(settings->value("TicksVisible").toBool());
    if(settings->contains("LabelsVisible"))
        setAxisLabelsVisible(settings->value("LabelsVisible").toBool());
    if(settings->contains("GridVisible"))
        setAxisGridVisible(settings->value("GridVisible").toBool());
    if(settings->contains("LineVisible"))
        setAxisLineVisible(settings->value("LineVisible").toBool());
    if(settings->contains("ScaleMode"))
        setAxisScale(AxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setAxisMaxCustom(settings->value("ScaleMax").toDouble());
    settings->endGroup();

    settings->beginGroup("Gauge");
    if(settings->contains("Position"))
        setGaugeVPosition(GaugeVPosition(settings->value("Position").toInt()));
    settings->endGroup();
}

void GaugeVertical::saveSettings(QSettings *settings)
{
    SingleDisplay::saveSettings(settings);

    settings->beginGroup("Gauge");
    settings->setValue("TankMode", mTankGauge);
    settings->endGroup();

    settings->beginGroup("Axis");
    settings->setValue("TicksVisible", mAxisTicksVisible);
    settings->setValue("LabelsVisible", mAxisLabelsVisible);
    settings->setValue("GridVisible", mAxisGridVisible);
    settings->setValue("LineVisible", mAxisLineVisible);
    settings->setValue("ScaleMode", mAxisScale);
    settings->setValue("ScaleMin", mAxisMinCustom);
    settings->setValue("ScaleMax", mAxisMaxCustom);
    settings->endGroup();

    settings->beginGroup("Gauge");
    settings->setValue("Position", mGaugeVPosition);
    settings->endGroup();
}

void GaugeVertical::buildElement()
{
    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mMainLayout->setRowSpacing(0);

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);
    mTextValue->setVisible(mValueVisible);

    switch(mGaugeVPosition) {
    case gpTop:
        mMainLayout->addElement(mBarLayout);
        mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);

        if(mHeaderVisible) {
            mMainLayout->addElement(mTextLabel);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
            mMainLayout->addElement(mTextUnit);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
        }

        if(mValueVisible) {
            mMainLayout->addElement(mTextValue);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
        }
        break;
    case gpMiddle:
        if(mHeaderVisible) {
            mMainLayout->addElement(mTextLabel);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
            mMainLayout->addElement(mTextUnit);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
        }

        mMainLayout->addElement(mBarLayout);
        mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);

        if(mValueVisible) {
            mMainLayout->addElement(mTextValue);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
        }
        break;
    case gpBottom:
        if(mHeaderVisible) {
            mMainLayout->addElement(mTextLabel);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
            mMainLayout->addElement(mTextUnit);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
        }

        if(mValueVisible) {
            mMainLayout->addElement(mTextValue);
            mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,0.001);
        }

        mMainLayout->addElement(mBarLayout);
        mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);
        break;
    }

    mMainLayout->needUpdate(true);
    mBarLayout->needUpdate(true);

    switch(mBackgroundMode) {
    case bmBackground:
        mAxisRect->setBackground(QApplication::palette().light().color());
        mAxisRectThresholds->setBackground(Qt::NoBrush);
        break;
    case bmTransparent:
        mAxisRect->setBackground(QApplication::palette().dark().color());
        mAxisRectThresholds->setBackground(Qt::NoBrush);
        break;
    }

    auto textColor = QApplication::palette().toolTipText().color();

    if(mAxisTicksVisible || mAxisLabelsVisible || mAxisGridVisible)
        mAxisRect->axis(QCPAxis::atRight)->setVisible(true);
    else
        mAxisRect->axis(QCPAxis::atRight)->setVisible(false);

    mAxisRect->axis(QCPAxis::atRight)->grid()->setVisible(mAxisGridVisible);
    mAxisRect->axis(QCPAxis::atRight)->setTickLabels(mAxisLabelsVisible);

    if(mAxisLineVisible) {
        mAxisRect->axis(QCPAxis::atRight)->setBasePen(QPen(textColor, 0));
    } else {
        mAxisRect->axis(QCPAxis::atRight)->setBasePen(Qt::NoPen);
    }

    if(mAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atRight)->setTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atRight)->setSubTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atRight)->setTickLengthOut(5);
        mAxisRect->axis(QCPAxis::atRight)->setSubTickLengthOut(2);
        mAxisRect->axis(QCPAxis::atRight)->setTickLabelPadding(3);
    } else {
        mAxisRect->axis(QCPAxis::atRight)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atRight)->setSubTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atRight)->setTickLengthOut(0);
        mAxisRect->axis(QCPAxis::atRight)->setSubTickLengthOut(0);
        mAxisRect->axis(QCPAxis::atRight)->setTickLabelPadding(0);
    }
}

void GaugeVertical::checkParameters()
{
    NumericalDisplay::checkParameters();
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);

    if(dashParam) {
        if(dashParam->getTimeSeries().parameterId() > 0) {
            return;
        }
    }

    mBar->data()->clear();

    switch(mBackgroundMode) {
    case bmBackground:
        mAxisRect->setBackground(QApplication::palette().light().color());
        mAxisRectThresholds->setBackground(Qt::NoBrush);
        break;
    case bmTransparent:
        mAxisRect->setBackground(QApplication::palette().dark().color());
        mAxisRectThresholds->setBackground(Qt::NoBrush);
        break;
    }
}

void GaugeVertical::resetElement()
{
    NumericalDisplay::resetElement();

    for(auto tracer : mThresholdTracers)
        mBoard->removeItem(tracer);
    mThresholdTracers.clear();

    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        QMap<double, ColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();

        QMap<double, ColorSettings>::iterator lowIt;
        double lower = -std::numeric_limits<double>::max();
        bool lowerFirst = true;
        for (lowIt = lowThr.begin();
             lowIt != lowThr.end(); ++lowIt) {

            auto rect = new QCPItemRect(mBoard);
            rect->topLeft->setType(QCPItemPosition::ptPlotCoords);

            if(lowerFirst)
                rect->bottomRight->setType(QCPItemPosition::ptAxisRectRatio);
            else
                rect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            rect->topLeft->setAxisRect(mAxisRectThresholds);
            rect->bottomRight->setAxisRect(mAxisRectThresholds);
            rect->topLeft->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atRight));
            rect->bottomRight->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atRight));
            rect->topLeft->setCoords(0, lowIt.key());

            if(lowerFirst)
                rect->bottomRight->setCoords(1,1);
            else
                rect->bottomRight->setCoords(1, lower);
            rect->setPen(Qt::NoPen);

            QColor color = lowIt.value().color();
            color.setAlpha(150);

            rect->setBrush(QBrush(color));
            rect->setClipAxisRect(mAxisRectThresholds);
            rect->setClipToAxisRect(true);

            mThresholdTracers.append(rect);

            lower = lowIt.key();
            lowerFirst = false;
        }

        QMap<double, ColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();

        QMapIterator<double, ColorSettings> highIt(highThr);
        highIt.toBack();
        double upper = std::numeric_limits<double>::max();
        bool upperFirst = true;
        while (highIt.hasPrevious()) {
            highIt.previous();

            auto rect = new QCPItemRect(mBoard);
            if(upperFirst)
                rect->topLeft->setType(QCPItemPosition::ptAxisRectRatio);
            else
                rect->topLeft->setType(QCPItemPosition::ptPlotCoords);
            rect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            rect->topLeft->setAxisRect(mAxisRectThresholds);
            rect->bottomRight->setAxisRect(mAxisRectThresholds);
            rect->topLeft->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atRight));
            rect->bottomRight->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atRight));
            if(upperFirst)
                rect->topLeft->setCoords(0, 0);
            else
                rect->topLeft->setCoords(0, upper);
            rect->bottomRight->setCoords(1, highIt.key());
            rect->setPen(Qt::NoPen);

            QColor color = highIt.value().color();
            color.setAlpha(150);

            rect->setBrush(QBrush(color));
            rect->setClipAxisRect(mAxisRectThresholds);
            rect->setClipToAxisRect(true);

            mThresholdTracers.append(rect);

            upper = highIt.key();
            upperFirst = false;
        }
    }

    for(int i=0; i<mBarLayout->elementCount(); i++)
        mBarLayout->takeAt(i);
    mBarLayout->simplify();

    if(mThresholdTracers.count() > 0) {

        mBarLayout->addElement(mBarLayoutEmptyElementFirst);
        mBarLayout->addElement(mAxisRectThresholds);
        mBarLayout->addElement(mAxisRect);
        mBarLayout->addElement(mBarLayoutEmptyElementSecond);

        mBarLayout->setColumnStretchFactor(0,0.01);
        mBarLayout->setColumnStretchFactor(3,0.01);

        mAxisRect->setMinimumMargins(QMargins(0, 10, 3, 10));
        mAxisRectThresholds->setMinimumMargins(QMargins(2, 10, 0, 10));
        mAxisRectThresholds->setVisible(true);
    } else {

        mBarLayout->addElement(mBarLayoutEmptyElementFirst);
        mBarLayout->addElement(mAxisRect);
        mBarLayout->addElement(mBarLayoutEmptyElementSecond);

        mBarLayout->setColumnStretchFactor(0,0.01);
        mBarLayout->setColumnStretchFactor(2,0.01);

        mAxisRect->setMinimumMargins(QMargins(2, 10, 2, 10));
        mAxisRectThresholds->setVisible(false);
    }
}

void GaugeVertical::displayData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        double val = 0.;
        if(dashParam->getTimeSeries().parameterId() > 0) {

            dashParam->processValueData();
            if(mValueVisible) {
                mTextValue->setText(dashParam->getValueString(mValueFormat));
                mTextValue->setTextColor(dashParam->getForegroundColor());
                mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
            }

            QColor colorBar;

            switch(dashParam->parameterConfiguration()->itemColorMode()) {
            case ParameterConfiguration::icBase:
                colorBar = (dashParam->parameterConfiguration()->defaultColorSettingsRef().color());
                break;
            case ParameterConfiguration::icDynamic:
            case ParameterConfiguration::icDynamicSegmented:
                colorBar = (dashParam->getColor());
                break;
            case ParameterConfiguration::icCustom:
                colorBar = (dashParam->parameterConfiguration()->itemStaticColor());
                break;
            }
            mBar->setBrush(colorBar);

            val = dashParam->getValueDouble();
            mBar->data()->clear();
            mBar->addData(1, val);

            if(val < mValueAxisRange.lower)
                mValueAxisRange.lower = val;
            if(val > mValueAxisRange.upper)
                mValueAxisRange.upper = val;
        }

        switch(mAxisScale) {
        case asAuto:
            mAxisRect->axis(QCPAxis::atRight)->setRange(mValueAxisRange);
            mAxisMinCustom = mValueAxisRange.lower;
            mAxisMaxCustom = mValueAxisRange.upper;
            break;
        case asParam:
        {
            if(dashParam->parameterConfiguration()->validRange()) {
                mAxisRect->axis(QCPAxis::atRight)->setRange(QCPRange(dashParam->parameterConfiguration()->rangeMinimum(), dashParam->parameterConfiguration()->rangeMaximum()));
            } else {
                mAxisRect->axis(QCPAxis::atRight)->setRange(mValueAxisRange);
                mAxisMinCustom = mValueAxisRange.lower;
                mAxisMaxCustom = mValueAxisRange.upper;
            }
            break;
        }
        case asCustom:
            mAxisRect->axis(QCPAxis::atRight)->setRange(QCPRange(mAxisMinCustom, mAxisMaxCustom));
            break;
        }

        mAxisRectThresholds->axis(QCPAxis::atRight)->setRange(mAxisRect->axis(QCPAxis::atRight)->range());

        if(mTankGauge && dashParam->getTimeSeries().parameterId() > 0) {
            QCPRange rangeTank(0, mAxisRect->axis(QCPAxis::atRight)->range().size());
            mBar->data()->clear();
            mBar->addData(1, val - mAxisRect->axis(QCPAxis::atRight)->range().lower);
            mAxisRect->axis(QCPAxis::atLeft)->setRange(rangeTank);
        } else {
            mAxisRect->axis(QCPAxis::atLeft)->setRange(mAxisRect->axis(QCPAxis::atRight)->range());
        }
    }
}

void GaugeVertical::loadHistoricalData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {

        QVector<DataValue> values;
        QVector<double> timestamps;
        double min = (std::numeric_limits<double>::max)();
        double max = -1. * (std::numeric_limits<double>::max)();

        if(mBoard->dataManager()->lastBoardData(dashParam->getTimeSeries().parameterId(), values, timestamps)) {
            QVector<DataValue>::const_iterator itValues;
            QVector<double>::const_iterator itTimestamps;

            for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                 (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                 ++itValues, ++itTimestamps) {

                auto val = itValues->toDouble();
                if(val < min)
                    min = val;
                if(val > max)
                    max = val;
            }

            mValueAxisRange.upper = max;
            mValueAxisRange.lower = min;
        }
        dashParam->updateData();
    }
}

void GaugeVertical::applySizeConstraints()
{
    NumericalDisplay::applySizeConstraints();
}

void GaugeVertical::resetThresholds()
{

}

bool GaugeVertical::tankGauge() const
{
    return mTankGauge;
}

void GaugeVertical::setTankGauge(bool tankGauge)
{
    mTankGauge = tankGauge;
}

double GaugeVertical::axisMinCustom() const
{
    return mAxisMinCustom;
}

void GaugeVertical::setAxisMinCustom(double axisMinCustom)
{
    mAxisMinCustom = axisMinCustom;
}

double GaugeVertical::axisMaxCustom() const
{
    return mAxisMaxCustom;
}

void GaugeVertical::setAxisMaxCustom(double axisMaxCustom)
{
    mAxisMaxCustom = axisMaxCustom;
}

GaugeVertical::AxisScale GaugeVertical::axisScale() const
{
    return mAxisScale;
}

void GaugeVertical::setAxisScale(const AxisScale &axisScale)
{
    mAxisScale = axisScale;
}

bool GaugeVertical::axisGridVisible() const
{
    return mAxisGridVisible;
}

void GaugeVertical::setAxisGridVisible(bool axisGridVisible)
{
    mAxisGridVisible = axisGridVisible;
}

GaugeVertical::GaugeVPosition GaugeVertical::gaugeVPosition() const
{
    return mGaugeVPosition;
}

void GaugeVertical::setGaugeVPosition(const GaugeVPosition &gaugePosition)
{
    mGaugeVPosition = gaugePosition;
}

bool GaugeVertical::axisLineVisible() const
{
    return mAxisLineVisible;
}

void GaugeVertical::setAxisLineVisible(bool axisLineVisible)
{
    mAxisLineVisible = axisLineVisible;
}

bool GaugeVertical::axisLabelsVisible() const
{
    return mAxisLabelsVisible;
}

void GaugeVertical::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
}

bool GaugeVertical::axisTicksVisible() const
{
    return mAxisTicksVisible;
}

void GaugeVertical::setAxisTicksVisible(bool axisTicksVisible)
{
    mAxisTicksVisible = axisTicksVisible;
}

int GaugeVertical::defaultWidth()
{
    return mBoard->dashboardLayout()->singleElementColumnCount();
}

int GaugeVertical::defaultHeight()
{
    return 4*mBoard->dashboardLayout()->singleElementRowCount();
}


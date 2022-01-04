#include "gauge_radial.h"
#include "gauge_radial_editor.h"

GaugeRadial::GaugeRadial(Board *dashboard) :
    NumericalDisplay (dashboard),
    mAxisScale(asAuto),
    mGaugePosition(gpMiddle),
    mAngleSpan(270),
    mAngleStart(0)
{
}

void GaugeRadial::clearElement()
{
    if(mGauge)
        delete mGauge;
    NumericalDisplay::clearElement();
}

void GaugeRadial::initialize(Board *dashboard)
{
    if(dashboard) {
        mGauge = new GaugeRect(dashboard);
        mGauge->axis()->setRange(QCPRange(0.,0.1));
        mGauge->axisRange().lower = 0.;
        mGauge->axisRange().upper = .1;
        mGauge->setAngleSpan(mAngleSpan);
        mGauge->setAngleStart(mAngleStart - 90);
        mGauge->axis()->setSpanAngle(mAngleSpan);
        mGauge->axis()->setAngle(mAngleStart - 90);

        NumericalDisplay::initialize(dashboard);
    }
}

void GaugeRadial::edit()
{
    GaugeRadialEditor editor(this);
    editor.exec();
}

void GaugeRadial::loadSettings(QSettings *settings)
{
    SingleDisplay::loadSettings(settings);

    settings->beginGroup("Axis");
    if(settings->contains("TicksVisible"))
        setAxisTicksVisible(settings->value("TicksVisible").toBool());
    if(settings->contains("LabelsVisible"))
        setAxisLabelsVisible(settings->value("LabelsVisible").toBool());
    if(settings->contains("LineVisible"))
        setAxisLineVisible(settings->value("LineVisible").toBool());
    if(settings->contains("ScaleMode"))
        setAxisScale(AxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setAxisMaxCustom(settings->value("ScaleMax").toDouble());
    if(settings->contains("AngleSpan"))
        setAngleSpan(settings->value("AngleSpan").toInt());
    if(settings->contains("AngleStart"))
        setAngleStart(settings->value("AngleStart").toInt());
    settings->endGroup();

    settings->beginGroup("Gauge");
    if(settings->contains("Position"))
        setGaugePosition(GaugeRPosition(settings->value("Position").toInt()));
    settings->endGroup();
}

void GaugeRadial::saveSettings(QSettings *settings)
{
    SingleDisplay::saveSettings(settings);

    settings->beginGroup("Axis");
    settings->setValue("TicksVisible", axisTicksVisible());
    settings->setValue("LineVisible", axisLineVisible());
    settings->setValue("LabelsVisible", axisLabelsVisible());
    settings->setValue("ScaleMode", axisScale());
    settings->setValue("ScaleMin", axisMinCustom());
    settings->setValue("ScaleMax", axisMaxCustom());
    settings->setValue("AngleSpan", angleSpan());
    settings->setValue("AngleStart", angleStart());
    settings->endGroup();

    settings->beginGroup("Gauge");
    settings->setValue("Position", mGaugePosition);
    settings->endGroup();
}

void GaugeRadial::buildElement()
{
    mGauge->setAngleSpan(mAngleSpan);
    mGauge->setAngleStart(mAngleStart - 90);
    mGauge->axis()->setSpanAngle(mAngleSpan);
    mGauge->axis()->setAngle(mAngleStart - 90);

    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mMainLayout->setRowSpacing(0);

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);
    mTextValue->setVisible(mValueVisible);

    switch(mGaugePosition) {
    case gpTop:
        mMainLayout->addElement(mGauge);
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

        mMainLayout->addElement(mGauge);
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

        mMainLayout->addElement(mGauge);
        mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);
        break;
    }

    mMainLayout->needUpdate(true);

    switch(mBackgroundMode) {
    case bmBackground:
        mGauge->setBackgroundColor(QApplication::palette().light().color());
        break;
    case bmTransparent:
        mGauge->setBackgroundColor(QApplication::palette().dark().color());
        break;
    }

    mGauge->clearThresholdBands();
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {

        QMap<double, ColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
        QMap<double, ColorSettings>::iterator lowIt;
        for (lowIt = lowThr.begin();
             lowIt != lowThr.end(); ++lowIt) {
            QColor color = lowIt.value().color();
            color.setAlpha(150);
            mGauge->addLowThreshold(color, lowIt.key());
        }

        QMap<double, ColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
        QMap<double, ColorSettings>::iterator highIt;
        for (highIt = highThr.begin();
             highIt != highThr.end(); ++highIt) {
            QColor color = highIt.value().color();
            color.setAlpha(150);
            mGauge->addHighThreshold(color, highIt.key());
        }
    }
}

void GaugeRadial::checkParameters()
{
    NumericalDisplay::checkParameters();

    mGauge->setEnabled(false);
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        if(dashParam->getTimeSeries().parameterId() > 0) {
            mGauge->setEnabled(true);
            return;
        }
    }
}

void GaugeRadial::displayData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        if(dashParam->getTimeSeries().parameterId() > 0) {
            dashParam->processValueData();
            if(mValueVisible) {
                mTextValue->setText(dashParam->getValueString(mValueFormat));
                mTextValue->setTextColor(dashParam->getForegroundColor());
                mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
            }

            QColor colorBar;
            double value = dashParam->getValueDouble();
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

            mGauge->setCurrentColor(colorBar);

            mGauge->setValue(value);

            if(value < mGauge->axisRange().lower)
                mGauge->axisRange().lower = value;
            if(value > mGauge->axisRange().upper)
                mGauge->axisRange().upper = value;
        }

        switch(mAxisScale) {
        case asAuto:
            mGauge->axis()->setRange(mGauge->axisRange());
            mAxisMinCustom = mGauge->axisRange().lower;
            mAxisMaxCustom = mGauge->axisRange().upper;
            break;
        case asParam:
        {
            if(dashParam->parameterConfiguration()->validRange()) {
                mGauge->axis()->setRange(QCPRange(dashParam->parameterConfiguration()->rangeMinimum(), dashParam->parameterConfiguration()->rangeMaximum()));
            } else {
                mGauge->axis()->setRange(mGauge->axisRange());
                mAxisMinCustom = mGauge->axisRange().lower;
                mAxisMaxCustom = mGauge->axisRange().upper;
            }
            break;
        }
        case asCustom:
            mGauge->axis()->setRange(QCPRange(mAxisMinCustom, mAxisMaxCustom));
            break;
        }

    }
}

void GaugeRadial::loadHistoricalData()
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

            mGauge->axisRange().lower = min;
            mGauge->axisRange().upper = max;
        }
        dashParam->updateData();
    }
}

GaugeRadial::AxisScale GaugeRadial::axisScale() const
{
    return mAxisScale;
}

double GaugeRadial::axisMaxCustom() const
{
    return mAxisMaxCustom;
}

double GaugeRadial::axisMinCustom() const
{
    return mAxisMinCustom;
}

void GaugeRadial::setAxisMinCustom(double axisMinCustom)
{
    mAxisMinCustom = axisMinCustom;
}

bool GaugeRadial::axisTicksVisible() const
{
    return mGauge->axisTicksVisible();
}

bool GaugeRadial::axisLineVisible() const
{
    return mGauge->axisLineVisible();
}

void GaugeRadial::setAxisTicksVisible(bool axisTicksVisible)
{
    mGauge->setAxisTicksVisible(axisTicksVisible);
}

void GaugeRadial::setAxisLineVisible(bool axisLineVisible)
{
    mGauge->setAxisLineVisible(axisLineVisible);
}

bool GaugeRadial::axisLabelsVisible() const
{
    return mGauge->axisLabelsVisible();
}

void GaugeRadial::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mGauge->setAxisLabelsVisible(axisLabelsVisible);
}

bool GaugeRadial::thresholdsVisible() const
{
    return mGauge->thresholdsVisible();
}

void GaugeRadial::setThresholdsVisible(bool thresholdsVisible)
{
    mGauge->setThresholdsVisible(thresholdsVisible);
}

void GaugeRadial::setAxisScale(const AxisScale &axisScale)
{
    mAxisScale = axisScale;
}

void GaugeRadial::setAxisMaxCustom(double axisMaxCustom)
{
    mAxisMaxCustom = axisMaxCustom;
}

GaugeRadial::GaugeRPosition GaugeRadial::gaugePosition() const
{
    return mGaugePosition;
}

void GaugeRadial::setGaugePosition(const GaugeRPosition &gaugePosition)
{
    mGaugePosition = gaugePosition;
}

int GaugeRadial::angleSpan() const
{
    return mAngleSpan;
}

void GaugeRadial::setAngleSpan(int angleSpan)
{
    mAngleSpan = angleSpan;
}

int GaugeRadial::angleStart() const
{
    return mAngleStart;
}

void GaugeRadial::setAngleStart(int angleStart)
{
    mAngleStart = angleStart;
}

int GaugeRadial::defaultWidth()
{
    return 3*mBoard->dashboardLayout()->singleElementColumnCount();
}

int GaugeRadial::defaultHeight()
{
    return 4*mBoard->dashboardLayout()->singleElementRowCount();
}


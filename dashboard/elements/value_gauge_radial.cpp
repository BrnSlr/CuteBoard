#include "value_gauge_radial.h"
#include "ui/element_editors/value_gauge_radial_editor.h"

QTBValueGaugeRadial::QTBValueGaugeRadial(QTBoard *dashboard) :
    QTBValueDisplay (dashboard),
    mAxisScale(asAuto)
{

}

void QTBValueGaugeRadial::clearElement()
{
    if(mGauge)
        delete mGauge;
    QTBValueDisplay::clearElement();
}

void QTBValueGaugeRadial::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {
        QTBDashboardElement::initializeElement(dashboard);

        mGauge = new QTBGaugeRect(dashboard);

        QTBValueDisplay::initializeElement(dashboard);
    }
}

void QTBValueGaugeRadial::edit()
{
    QTBValueGaugeRadialEditor editor(this);
    editor.exec();
}

void QTBValueGaugeRadial::loadSettings(QSettings *settings)
{
    QTBSingleDisplay::loadSettings(settings);

    settings->beginGroup("Axis");
    if(settings->contains("TicksVisible"))
        setAxisTicksVisible(settings->value("TicksVisible").toBool());
    if(settings->contains("LabelsVisible"))
        setAxisLabelsVisible(settings->value("LabelsVisible").toBool());
    if(settings->contains("Style"))
        setRadialStyle(QTBGaugeRect::GaugeStyle(settings->value("Style").toInt()));
    if(settings->contains("ScaleMode"))
        setAxisScale(AxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setAxisMaxCustom(settings->value("ScaleMax").toDouble());
    settings->endGroup();
}

void QTBValueGaugeRadial::saveSettings(QSettings *settings)
{
    QTBSingleDisplay::saveSettings(settings);

    settings->beginGroup("Axis");
    settings->setValue("TicksVisible", axisTicksVisible());
    settings->setValue("LabelsVisible", axisLabelsVisible());
    settings->setValue("Style", radialStyle());
    settings->setValue("ScaleMode", axisScale());
    settings->setValue("ScaleMin", axisMinCustom());
    settings->setValue("ScaleMax", axisMaxCustom());
    settings->endGroup();
}

void QTBValueGaugeRadial::updateElement()
{
    QTBValueDisplay::updateElement();

    mGauge->clearThresholdBands();
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam) {

        QMap<double, QTBColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
        QMap<double, QTBColorSettings>::iterator lowIt;
        for (lowIt = lowThr.begin();
             lowIt != lowThr.end(); ++lowIt) {
            QColor color = lowIt.value().color();
            color.setAlpha(150);
            mGauge->addLowThreshold(color, lowIt.key());
        }

        QMap<double, QTBColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
        QMap<double, QTBColorSettings>::iterator highIt;
        for (highIt = highThr.begin();
             highIt != highThr.end(); ++highIt) {
            QColor color = highIt.value().color();
            color.setAlpha(150);
            mGauge->addLowThreshold(color, highIt.key());
        }
    }

}

void QTBValueGaugeRadial::buildLayout()
{
    mSubLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);

    mSubLayout->setRowSpacing(0);
    mMainLayout->setRowSpacing(0);

    mSubLayout->addElement(mTextLabel);
    mSubLayout->addElement(mTextUnit);

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);
    mTextValue->setVisible(mValueVisible);

    if(mHeaderVisible)
        mMainLayout->addElement(mSubLayout);

    mMainLayout->addElement(mGauge);

    if(mValueVisible)
        mMainLayout->addElement(mTextValue);

    mSubLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void QTBValueGaugeRadial::processNewSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
     if(dashParam && dashParam->getParameterId() > 0) {
        if(mValueVisible) {
            mTextValue->setText(dashParam->getValueString(mValueFormat));
            mTextValue->setTextColor(dashParam->getForegroundColor());
            mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
        }

        double value = dashParam->getValueDouble();
        mGauge->setCurrentColor(dashParam->getColor());
        mGauge->setValue(value);

        if(value < mGauge->axisRange().lower)
            mGauge->axisRange().lower = value;
        if(value > mGauge->axisRange().upper)
            mGauge->axisRange().upper = value;

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

    } else {
        mTextValue->setText("X");
    }

}

void QTBValueGaugeRadial::updateSizeConstraints()
{
    if(mValueVisible) {
        QSizeF singleElementSize = mBoard->dashboardLayout()->singleElementSize();
        QSizeF labelSize = singleElementSize;
        if(mRect.height() < singleElementSize.height())
            labelSize.setHeight(mRect.height());

        mTextValue->setMaximumSize(mRect.width(), 0.5 * labelSize.height());
    }

    QTBValueDisplay::updateSizeConstraints();
}

void QTBValueGaugeRadial::checkSizeAndVisibility()
{
    if(mValueVisible) {
        if(!mTextValue->visible()) {
            mTextValue->setMaximumSize(mRect.width(), 0);
            mMainLayout->needUpdate(true);
        }
    }
    QTBValueDisplay::checkSizeAndVisibility();
}

QTBValueGaugeRadial::AxisScale QTBValueGaugeRadial::axisScale() const
{
    return mAxisScale;
}

double QTBValueGaugeRadial::axisMaxCustom() const
{
    return mAxisMaxCustom;
}

double QTBValueGaugeRadial::axisMinCustom() const
{
    return mAxisMinCustom;
}

void QTBValueGaugeRadial::setAxisMinCustom(double axisMinCustom)
{
    mAxisMinCustom = axisMinCustom;
}

bool QTBValueGaugeRadial::axisTicksVisible() const
{
    return mGauge->axisTicksVisible();
}

void QTBValueGaugeRadial::setAxisTicksVisible(bool axisTicksVisible)
{
    mGauge->setAxisTicksVisible(axisTicksVisible);
}

bool QTBValueGaugeRadial::axisLabelsVisible() const
{
    return mGauge->axisLabelsVisible();
}

void QTBValueGaugeRadial::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mGauge->setAxisLabelsVisible(axisLabelsVisible);
}

bool QTBValueGaugeRadial::thresholdsVisible() const
{
    return mGauge->thresholdsVisible();
}

void QTBValueGaugeRadial::setThresholdsVisible(bool thresholdsVisible)
{
    mGauge->setThresholdsVisible(thresholdsVisible);
}

QTBGaugeRect::GaugeStyle QTBValueGaugeRadial::radialStyle() const
{
    return mGauge->gaugeStyle();
    }

void QTBValueGaugeRadial::setAxisScale(const AxisScale &axisScale)
{
    mAxisScale = axisScale;
}

void QTBValueGaugeRadial::setAxisMaxCustom(double axisMaxCustom)
{
    mAxisMaxCustom = axisMaxCustom;
    }

void QTBValueGaugeRadial::setRadialStyle(const QTBGaugeRect::GaugeStyle &radialStyle)
{
    mGauge->setGaugeStyle(radialStyle);
}

int QTBValueGaugeRadial::defaultWidth()
{
    return 2*mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBValueGaugeRadial::defaultHeight()
{
    return 4*mBoard->dashboardLayout()->singleElementRowCount();
}


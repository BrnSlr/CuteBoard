#include "value_gauge_vertical.h"
#include "ui/element_editors/value_gauge_vertical_editor.h"
#include "project/project.h"

QTBValueGaugeVertical::QTBValueGaugeVertical(QTBoard *dashboard) :
    QTBValueDisplay (dashboard),
    mValueAxisRange(QCPRange(0,0)),
    mAxisTicksVisible(true),
    mAxisLabelsVisible(true),
    mAxisGridVisible(true),
    mAxisScale(asAuto),
    mTankGauge(true)
{

}

void QTBValueGaugeVertical::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {

        mAxisRect = new QTBAxisRect(dashboard);

        mAxisRect->axis(QCPAxis::atLeft)->setVisible(true);
        mAxisRect->axis(QCPAxis::atBottom)->setVisible(false);

        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(QPen(dashboard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(QPen(dashboard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(QPen(dashboard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atLeft)->setTickLabelColor(dashboard->frontColor());

        mAxisRect->axis(QCPAxis::atLeft)->setTickLabels(true);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(false);

        mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(true);
        mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(false);

        mAxisRect->axis(QCPAxis::atLeft)->setOffset(5);
        mAxisRect->axis(QCPAxis::atLeft)->setPadding(1);
        mAxisRect->axis(QCPAxis::atLeft)->setTickLabelPadding(3);

        mAxisRect->axis(QCPAxis::atBottom)->setRange(0.5, 1.5);
        mAxisRect->setMinimumMargins(QMargins(50, 15, 50, 15));

        mBar = new QCPBars(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
        mBar->setWidth(1);
        mBar->setAntialiased(false);
        mBar->setAntialiasedFill(false);
        mBar->setPen(Qt::NoPen);
        mBar->setBrush(QColor(0,131,179));

        QTBValueDisplay::initializeElement(dashboard);
    }
}

void QTBValueGaugeVertical::buildLayout()
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

    mMainLayout->addElement(mAxisRect);

    if(mValueVisible)
        mMainLayout->addElement(mTextValue);

    mSubLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void QTBValueGaugeVertical::clearElement()
{
    if(mAxisRect) {
        mAxisRect->clearPlottables();
        delete mAxisRect;
    }
    QTBValueDisplay::clearElement();
}

void QTBValueGaugeVertical::edit()
{
    QTBValueGaugeVerticalEditor editor(this);
    editor.exec();
}

void QTBValueGaugeVertical::loadSettings(QSettings *settings)
{
    QTBSingleDisplay::loadSettings(settings);

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
    if(settings->contains("ScaleMode"))
        setAxisScale(AxisScale(settings->value("ScaleMode").toInt()));
    if(settings->contains("ScaleMin"))
        setAxisMinCustom(settings->value("ScaleMin").toDouble());
    if(settings->contains("ScaleMax"))
        setAxisMaxCustom(settings->value("ScaleMax").toDouble());
    settings->endGroup();
}

void QTBValueGaugeVertical::saveSettings(QSettings *settings)
{
    QTBSingleDisplay::saveSettings(settings);

    settings->beginGroup("Gauge");
    settings->setValue("TankMode", mTankGauge);
    settings->endGroup();

    settings->beginGroup("Axis");
    settings->setValue("TicksVisible", mAxisTicksVisible);
    settings->setValue("LabelsVisible", mAxisLabelsVisible);
    settings->setValue("GridVisible", mAxisGridVisible);
    settings->setValue("ScaleMode", mAxisScale);
    settings->setValue("ScaleMin", mAxisMinCustom);
    settings->setValue("ScaleMax", mAxisMaxCustom);
    settings->endGroup();
}

void QTBValueGaugeVertical::processNewSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam && dashParam->getParameterId() > 0) {

        if(mValueVisible) {
            mTextValue->setText(dashParam->getValueString(mValueFormat));
            mTextValue->setTextColor(dashParam->getForegroundColor());
            mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
        }

        double val = dashParam->getValueDouble();
        mBar->data()->clear();
        mBar->addData(1, val);

        mBar->setBrush(QBrush(dashParam->getColor()));

        if(val < mValueAxisRange.lower)
            mValueAxisRange.lower = val;
        if(val > mValueAxisRange.upper)
            mValueAxisRange.upper = val;

        switch(mAxisScale) {
        case asAuto:
            mAxisRect->axis(QCPAxis::atLeft)->setRange(mValueAxisRange);
            mAxisMinCustom = mValueAxisRange.lower;
            mAxisMaxCustom = mValueAxisRange.upper;
            break;
        case asParam:
        {
            if(dashParam->parameterConfiguration()->validRange()) {
                mAxisRect->axis(QCPAxis::atLeft)->setRange(QCPRange(dashParam->parameterConfiguration()->rangeMinimum(), dashParam->parameterConfiguration()->rangeMaximum()));
            } else {
                mAxisRect->axis(QCPAxis::atLeft)->setRange(mValueAxisRange);
                mAxisMinCustom = mValueAxisRange.lower;
                mAxisMaxCustom = mValueAxisRange.upper;
            }
            break;
        }
        case asCustom:
            mAxisRect->axis(QCPAxis::atLeft)->setRange(QCPRange(mAxisMinCustom, mAxisMaxCustom));
            break;
        }

        if(mTankGauge) {
            QCPRange rangeTank(0, mAxisRect->axis(QCPAxis::atLeft)->range().size());
            mBar->data()->clear();
            mBar->addData(1, val - mAxisRect->axis(QCPAxis::atLeft)->range().lower);
            mAxisRect->axis(QCPAxis::atRight)->setRange(rangeTank);
        } else {
            mAxisRect->axis(QCPAxis::atRight)->setRange(mAxisRect->axis(QCPAxis::atLeft)->range());
        }
    } else {
        mTextValue->setText("X");
        mBar->data()->clear();
    }
}

void QTBValueGaugeVertical::updateSizeConstraints()
{
    if(mValueVisible) {
        QSizeF singleElementSize = mBoard->dashboardLayout()->singleElementSize();
        QSizeF labelSize = singleElementSize;
        if(mRect.height() < singleElementSize.height())
            labelSize.setHeight(mRect.height());

        mTextValue->setMinimumSize(mRect.width(), 0.5 * labelSize.height());
        mTextValue->setMaximumSize(mRect.width(), 0.5 * labelSize.height());
    }

    QTBValueDisplay::updateSizeConstraints();
}

void QTBValueGaugeVertical::checkSizeAndVisibility()
{
//    if(mValueVisible) {
//        if(!mTextValue->visible()) {
//            mTextValue->setMaximumSize(mRect.width(), 0);
//            mMainLayout->needUpdate(true);
//        }
//    }
    QTBValueDisplay::checkSizeAndVisibility();
}

bool QTBValueGaugeVertical::tankGauge() const
{
    return mTankGauge;
}

void QTBValueGaugeVertical::setTankGauge(bool tankGauge)
{
    mTankGauge = tankGauge;
}

double QTBValueGaugeVertical::axisMinCustom() const
{
    return mAxisMinCustom;
}

void QTBValueGaugeVertical::setAxisMinCustom(double axisMinCustom)
{
    mAxisMinCustom = axisMinCustom;
}

double QTBValueGaugeVertical::axisMaxCustom() const
{
    return mAxisMaxCustom;
}

void QTBValueGaugeVertical::setAxisMaxCustom(double axisMaxCustom)
{
    mAxisMaxCustom = axisMaxCustom;
}

QTBValueGaugeVertical::AxisScale QTBValueGaugeVertical::axisScale() const
{
    return mAxisScale;
}

void QTBValueGaugeVertical::setAxisScale(const AxisScale &axisScale)
{
    mAxisScale = axisScale;
}

bool QTBValueGaugeVertical::axisGridVisible() const
{
    return mAxisGridVisible;
}

void QTBValueGaugeVertical::setAxisGridVisible(bool axisGridVisible)
{
    mAxisGridVisible = axisGridVisible;
    mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(mAxisGridVisible);
}

bool QTBValueGaugeVertical::axisLabelsVisible() const
{
    return mAxisLabelsVisible;
}

void QTBValueGaugeVertical::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
    mAxisRect->axis(QCPAxis::atLeft)->setTickLabels(mAxisLabelsVisible);
}

bool QTBValueGaugeVertical::axisTicksVisible() const
{
    return mAxisTicksVisible;
}

void QTBValueGaugeVertical::setAxisTicksVisible(bool axisTicksVisible)
{
    mAxisTicksVisible = axisTicksVisible;
    if(mAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(QPen(mBoard->frontColor()));
    } else {
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(Qt::NoPen);
    }
}

int QTBValueGaugeVertical::defaultWidth()
{
    return mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBValueGaugeVertical::defaultHeight()
{
    return 4*mBoard->dashboardLayout()->singleElementRowCount();
}


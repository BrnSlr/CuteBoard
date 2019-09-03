#include "value_gauge_radial.h"
#include "ui/element_editors/value_gauge_radial_editor.h"

QTBValueGaugeRadial::QTBValueGaugeRadial(QTBoard *dashboard) :
    QTBValueDisplay (dashboard),
    mAxisRange(QCPRange(0,0)),
    mValueAngle(0),
    mAxisTicksVisible(true),
    mAxisLabelsVisible(true),
    mAxisScale(asAuto),
    mNeedleVisible(true),
    mThresholdsVisible(true)
{

}

void QTBValueGaugeRadial::clearElement()
{
    if(mAxis)
        delete mAxis;
    QTBValueDisplay::clearElement();
}

void QTBValueGaugeRadial::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {
        QTBDashboardElement::initializeElement(dashboard);

        mAxis = new QTBCircularAxis(dashboard);
        mAxis->setRange(-0.01,0.01);
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
    settings->setValue("TicksVisible", mAxisTicksVisible);
    settings->setValue("LabelsVisible", mAxisLabelsVisible);
    settings->setValue("ScaleMode", mAxisScale);
    settings->setValue("ScaleMin", mAxisMinCustom);
    settings->setValue("ScaleMax", mAxisMaxCustom);
    settings->endGroup();
}

void QTBValueGaugeRadial::updateElement()
{
    QTBValueDisplay::updateElement();

    mLowThresholdsBandColors.clear();
    mHighThresholdsBandColors.clear();
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam) {

        QMap<double, QTBColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();
        QMap<double, QTBColorSettings>::iterator lowIt;
        for (lowIt = lowThr.begin();
             lowIt != lowThr.end(); ++lowIt) {
            QColor color = lowIt.value().color();
            color.setAlpha(150);
            mLowThresholdsBandColors.append(QPair<QColor, double>(color, lowIt.key()));
        }

        QMap<double, QTBColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();
        QMap<double, QTBColorSettings>::iterator highIt;
        for (highIt = highThr.begin();
             highIt != highThr.end(); ++highIt) {
            QColor color = highIt.value().color();
            color.setAlpha(150);
            mHighThresholdsBandColors.append(QPair<QColor, double>(color, highIt.key()));
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

    mMainLayout->addElement(mAxis);

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

        mNeedleBrush = QBrush(dashParam->getColor());

        double val = dashParam->getValueDouble();
        if(val < mAxisRange.lower)
            mAxisRange.lower = val;
        if(val > mAxisRange.upper)
            mAxisRange.upper = val;

        switch(mAxisScale) {
        case asAuto:
            mAxis->setRange(mAxisRange);
            mAxisMinCustom = mAxisRange.lower;
            mAxisMaxCustom = mAxisRange.upper;
            break;
        case asParam:
        {
            if(dashParam->parameterConfiguration()->validRange()) {
                mAxis->setRange(QCPRange(dashParam->parameterConfiguration()->rangeMinimum(), dashParam->parameterConfiguration()->rangeMaximum()));
            } else {
                mAxis->setRange(mAxisRange);
                mAxisMinCustom = mAxisRange.lower;
                mAxisMaxCustom = mAxisRange.upper;
            }
            break;
        }
        case asCustom:
            mAxis->setRange(QCPRange(mAxisMinCustom, mAxisMaxCustom));
            break;
        }

        mValueAngle = mAxis->coordToAngleRad(dashParam->getValueDouble()) - mAxis->angleRad();
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

void QTBValueGaugeRadial::setAxisScale(const AxisScale &axisScale)
{
    mAxisScale = axisScale;
}

double QTBValueGaugeRadial::axisMaxCustom() const
{
    return mAxisMaxCustom;
}

void QTBValueGaugeRadial::setAxisMaxCustom(double axisMaxCustom)
{
    mAxisMaxCustom = axisMaxCustom;
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
    return mAxisTicksVisible;
}

void QTBValueGaugeRadial::setAxisTicksVisible(bool axisTicksVisible)
{
    mAxisTicksVisible = axisTicksVisible;
    if(mAxisTicksVisible) {
        mAxis->setBasePen(QPen(mAxis->baseColor()));
        mAxis->setTickPen(QPen(mAxis->baseColor()));
        mAxis->setSubTickPen(QPen(mAxis->baseColor()));
    } else {
        mAxis->setBasePen(Qt::NoPen);
        mAxis->setTickPen(Qt::NoPen);
        mAxis->setSubTickPen(Qt::NoPen);
    }
}

bool QTBValueGaugeRadial::axisLabelsVisible() const
{
    return mAxisLabelsVisible;
}

void QTBValueGaugeRadial::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
    mAxis->setTickLabels(mAxisLabelsVisible);
}

bool QTBValueGaugeRadial::thresholdsVisible() const
{
    return mThresholdsVisible;
}

void QTBValueGaugeRadial::setThresholdsVisible(bool thresholdsVisible)
{
    mThresholdsVisible = thresholdsVisible;
}

void QTBValueGaugeRadial::drawColorBands(QCPPainter *painter)
{
    QPen pen;
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(mAxis->radius() / 20.0);
    painter->setBrush(Qt::NoBrush);

    QRectF bandRect;
    bandRect.setWidth(0.8*2*mAxis->radius());
    bandRect.setHeight(0.8*2*mAxis->radius());
    bandRect.moveCenter(mAxis->center());

    for (int i = 0; i<mLowThresholdsBandColors.size(); i++) {
        int startAngle, currentAngle;
        if(i==0)
            startAngle = int(-1.0*mAxis->angleRad() * 180.0 / M_PI) * 16;
        else
            startAngle = int(-1.0*mAxis->coordToAngleRad(mLowThresholdsBandColors[i-1].second)* 180.0 / M_PI) * 16;

        currentAngle = int(-1.0*mAxis->coordToAngleRad(mLowThresholdsBandColors[i].second)* 180.0 / M_PI) * 16;

        pen.setColor(mLowThresholdsBandColors[i].first);
        painter->setPen(pen);
        painter->drawArc(bandRect, startAngle, currentAngle - startAngle);
    }

    for (int i = mHighThresholdsBandColors.size() - 1; i>=0; i--) {
        int startAngle, currentAngle;

        startAngle = int(-1.0*mAxis->coordToAngleRad(mHighThresholdsBandColors[i].second)* 180.0 / M_PI) * 16;

        if(i==mHighThresholdsBandColors.size()-1)
            currentAngle = int(-1.0*mAxis->coordToAngleRad(mAxis->range().upper)* 180.0 / M_PI) * 16;
        else
            currentAngle = int(-1.0*mAxis->coordToAngleRad(mHighThresholdsBandColors[i+1].second)* 180.0 / M_PI) * 16;

        pen.setColor(mHighThresholdsBandColors[i].first);
        painter->setPen(pen);
        painter->drawArc(bandRect, startAngle, currentAngle - startAngle);
    }
}

void QTBValueGaugeRadial::draw(QCPPainter *painter)
{
    QTBDashboardElement::draw(painter);

    if(mThresholdsVisible) {
        drawColorBands(painter);
    }

    if(mNeedleVisible) {
        if(!qIsNaN(mValueAngle)) {
            double valueDeg = 45.0 + mValueAngle * 180.0 / M_PI;

            painter->save();
            painter->translate(mAxis->center());
            painter->rotate(valueDeg);
            painter->setBrush(mNeedleBrush);

            painter->setPen(Qt::NoPen);

            QVector<QPointF> tmpPoints;
            tmpPoints.append(QPointF(0.0, 0.0));
            tmpPoints.append(QPointF(-mAxis->radius() / 20.0, mAxis->radius() / 20.0));
            tmpPoints.append(QPointF(0.0, mAxis->radius()));
            tmpPoints.append(QPointF(mAxis->radius() / 20.0, mAxis->radius() / 20.0));

            painter->drawConvexPolygon(tmpPoints);
            painter->restore();
        }
    }
}

int QTBValueGaugeRadial::defaultWidth()
{
    return 2*mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBValueGaugeRadial::defaultHeight()
{
    return 4*mBoard->dashboardLayout()->singleElementRowCount();
}


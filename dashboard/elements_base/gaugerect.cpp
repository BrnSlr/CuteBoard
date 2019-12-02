#include "gaugerect.h"

QTBGaugeRect::QTBGaugeRect(QTBoard *dashboard):
    QCPLayoutElement(dashboard),
    mAxisRange(QCPRange(0,0)),
    mValue(0),
    mGaugeStyle(gsNeedle),
    mAxisTicksVisible(true),
    mAxisLabelsVisible(true),
    mThresholdsVisible(true)
{
    setLayer(QLatin1String("main"));

    mAxis = new QTBCircularAxis(dashboard);
    mAxis->setRange(-0.1,0.1);
    mAxis->setTickLengthOut(mAxis->tickLengthIn());
    mAxis->setTickLengthIn(0);
    mAxis->setSubTickLengthOut(mAxis->subTickLengthIn());
    mAxis->setSubTickLengthIn(0);
    mAxis->setParentLayerable(this);
}

QTBGaugeRect::~QTBGaugeRect()
{
    if(mAxis)
        delete mAxis;
}

void QTBGaugeRect::update(QCPLayoutElement::UpdatePhase phase)
{
    if(phase == upLayout)
        mAxis->setOuterRect(mRect);

    mAxis->update(phase);
}

void QTBGaugeRect::clearThresholdBands()
{
    mLowThresholdsBandColors.clear();
    mHighThresholdsBandColors.clear();
}

void QTBGaugeRect::addLowThreshold(QColor color, double value)
{
    mLowThresholdsBandColors.append(QPair<QColor, double>(color, value));
}

void QTBGaugeRect::addHighThreshold(QColor color, double value)
{
    mHighThresholdsBandColors.append(QPair<QColor, double>(color, value));
}

QTBGaugeRect::GaugeStyle QTBGaugeRect::gaugeStyle() const
{
    return mGaugeStyle;
}

void QTBGaugeRect::setGaugeStyle(const GaugeStyle &gaugeStyle)
{
    mGaugeStyle = gaugeStyle;
}

bool QTBGaugeRect::thresholdsVisible() const
{
    return mThresholdsVisible;
}

void QTBGaugeRect::setThresholdsVisible(bool thresholdsVisible)
{
    mThresholdsVisible = thresholdsVisible;
}

bool QTBGaugeRect::axisLabelsVisible() const
{
    return mAxisLabelsVisible;
}

void QTBGaugeRect::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
    mAxis->setTickLabels(mAxisLabelsVisible);
}

bool QTBGaugeRect::axisTicksVisible() const
{
    return mAxisTicksVisible;
}

void QTBGaugeRect::setAxisTicksVisible(bool axisTicksVisible)
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

void QTBGaugeRect::setCurrentColor(const QColor &currentColor)
{
    mCurrentColor = currentColor;
}

void QTBGaugeRect::setValue(double value)
{
    mValue = value;    
}

QCPRange & QTBGaugeRect::axisRange()
{
    return mAxisRange;
}

QTBCircularAxis *QTBGaugeRect::axis() const
{
    return mAxis;
}

void QTBGaugeRect::drawColorBands(QCPPainter *painter)
{
    QPen pen;
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(mAxis->radius() / 20.0);
    painter->setBrush(Qt::NoBrush);

    QRectF bandRect;
    bandRect.setWidth(0.7*2*mAxis->radius());
    bandRect.setHeight(0.7*2*mAxis->radius());
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

void QTBGaugeRect::drawNeedle(QCPPainter *painter)
{
    if(!qIsNaN(mValue)) {
        double valueAngle = mAxis->coordToAngleRad(mValue) - mAxis->angleRad();
        double valueDeg = 45.0 + valueAngle * 180.0 / M_PI;

        painter->save();
        painter->translate(mAxis->center());
        painter->rotate(valueDeg);
        painter->setBrush(QBrush(mCurrentColor));

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

void QTBGaugeRect::drawBand(QCPPainter *painter)
{
    QPen pen;
    pen.setColor(mCurrentColor);
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(mAxis->radius() / 10.0);
    painter->setBrush(Qt::NoBrush);

    QRectF bandRect;
    bandRect.setWidth(0.9*2*mAxis->radius());
    bandRect.setHeight(0.9*2*mAxis->radius());
    bandRect.moveCenter(mAxis->center());

    int startAngle = int(-1.0*mAxis->angleRad() * 180.0 / M_PI) * 16;
    int currentAngle = int(-1.0*mAxis->coordToAngleRad(mValue)* 180.0 / M_PI) * 16;
    painter->setPen(pen);
    painter->drawArc(bandRect, startAngle, currentAngle - startAngle);
}

void QTBGaugeRect::draw(QCPPainter *painter)
{
    if(mThresholdsVisible) {
        drawColorBands(painter);
    }

    if(mGaugeStyle == gsNeedle)
        drawNeedle(painter);
    else
        drawBand(painter);
}

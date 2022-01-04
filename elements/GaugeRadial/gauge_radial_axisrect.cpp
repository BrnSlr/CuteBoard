#include "gauge_radial_axisrect.h"
#include "ui/board/management/color_settings.h"

GaugeRect::GaugeRect(Board *dashboard):
    QCPLayoutElement(dashboard),
    mBoard(dashboard),
    mAxisRange(QCPRange(0,0)),
    mCurrentColor(QColor(255, 255, 255, 0)),
    mBackgroundColor(QApplication::palette().light().color()),
    mValue(0),
    mAxisTicksVisible(true),
    mAxisLineVisible(false),
    mAxisLabelsVisible(true),
    mThresholdsVisible(true),
    mEnabled(false),
    mAngleSpan(270),
    mAngleStart(135)
{
    setLayer(QLatin1String("main"));

    mAxis = new CircularAxis(dashboard);
    mAxis->setRange(-0.1,0.1);
    mAxis->setTickLengthOut(mAxis->tickLengthIn());
    mAxis->setTickLengthIn(0);
    mAxis->setSubTickLengthOut(mAxis->subTickLengthIn());
    mAxis->setSubTickLengthIn(0);
    mAxis->setBasePen(Qt::NoPen);

    mNeedleColor = QApplication::palette().toolTipText().color();
}

GaugeRect::~GaugeRect()
{
    if(mAxis)
        delete mAxis;
}

void GaugeRect::update(QCPLayoutElement::UpdatePhase phase)
{
    if(phase == upLayout)
        mAxis->setOuterRect(mRect);
    mAxis->update(phase);
}

void GaugeRect::clearThresholdBands()
{
    mLowThresholdsBandColors.clear();
    mHighThresholdsBandColors.clear();
}

void GaugeRect::addLowThreshold(QColor color, double value)
{
    mLowThresholdsBandColors.append(QPair<QColor, double>(color, value));
}

void GaugeRect::addHighThreshold(QColor color, double value)
{
    mHighThresholdsBandColors.append(QPair<QColor, double>(color, value));
}

bool GaugeRect::thresholdsVisible() const
{
    return mThresholdsVisible;
}

void GaugeRect::setThresholdsVisible(bool thresholdsVisible)
{
    mThresholdsVisible = thresholdsVisible;
}

bool GaugeRect::axisLabelsVisible() const
{
    return mAxisLabelsVisible;
}

void GaugeRect::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
    mAxis->setTickLabels(mAxisLabelsVisible);
}

bool GaugeRect::axisTicksVisible() const
{
    return mAxisTicksVisible;
}

void GaugeRect::setAxisTicksVisible(bool axisTicksVisible)
{
    mAxisTicksVisible = axisTicksVisible;
    if(mAxisTicksVisible) {
        mAxis->setTickPen(QPen(mAxis->baseColor(), 0));
        mAxis->setSubTickPen(QPen(mAxis->baseColor(), 0));
    } else {
        mAxis->setTickPen(Qt::NoPen);
        mAxis->setSubTickPen(Qt::NoPen);
    }
}

void GaugeRect::setCurrentColor(const QColor &currentColor)
{
    mCurrentColor = currentColor;
}

void GaugeRect::setValue(double value)
{
    mValue = value;
}

QCPRange & GaugeRect::axisRange()
{
    return mAxisRange;
}

CircularAxis *GaugeRect::axis() const
{
    return mAxis;
}

bool GaugeRect::axisLineVisible() const
{
    return mAxisLineVisible;
}

void GaugeRect::setAxisLineVisible(bool axisLineVisible)
{
    mAxisLineVisible = axisLineVisible;
    if(mAxisLineVisible) {
        mAxis->setBasePen(QPen(mAxis->baseColor(), 0));
    } else {
        mAxis->setBasePen(Qt::NoPen);
    }
}

QColor GaugeRect::backgroundColor() const
{
    return mBackgroundColor;
}

void GaugeRect::setBackgroundColor(const QColor &backgroundColor)
{
    mBackgroundColor = backgroundColor;
}

bool GaugeRect::enabled() const
{
    return mEnabled;
}

void GaugeRect::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

int GaugeRect::angleSpan() const
{
    return mAngleSpan;
}

void GaugeRect::setAngleSpan(int angleSpan)
{
    mAngleSpan = angleSpan;
}

int GaugeRect::angleStart() const
{
    return mAngleStart;
}

void GaugeRect::setAngleStart(int angleStart)
{
    mAngleStart = angleStart;
}

void GaugeRect::drawColorBands(QCPPainter *painter)
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

void GaugeRect::drawNeedle(QCPPainter *painter)
{
    if(!qIsNaN(mValue)) {
        double valueAngle = mAxis->coordToAngleRad(mValue);
        double valueDeg = valueAngle * 180.0 / M_PI - 90;

        painter->save();
        painter->translate(mAxis->center());
        painter->rotate(valueDeg);
        painter->setBrush(QBrush(mNeedleColor));
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

void GaugeRect::drawCenter(QCPPainter *painter)
{
    QRectF centerRect;
    centerRect.setWidth(0.3*2*mAxis->radius());
    centerRect.setHeight(0.3*2*mAxis->radius());
    centerRect.moveCenter(mAxis->center());

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(mBackgroundColor));
    painter->drawEllipse(centerRect);
}

void GaugeRect::drawCenterBackground(QCPPainter *painter)
{
    QRectF centerRect;
    centerRect.setWidth(0.45*2*mAxis->radius());
    centerRect.setHeight(0.45*2*mAxis->radius());
    centerRect.moveCenter(mAxis->center());

    QRadialGradient gradient(mAxis->center(),
                             0.45*mAxis->radius(),
                             mAxis->center());

    QColor transp = mBackgroundColor;
    transp.setAlpha(0);
    gradient.setColorAt(0, transp);
    gradient.setColorAt(0.85, transp);
    gradient.setColorAt(1, mBackgroundColor);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(gradient));
    painter->drawEllipse(centerRect);
}

void GaugeRect::drawBackgroundBand(QCPPainter *painter)
{
    QPen pen;
    pen.setColor(mBackgroundColor);
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(mAxis->radius() / 10.0);
    painter->setBrush(Qt::NoBrush);

    QRectF bandRect;
    bandRect.setWidth(0.8*2*mAxis->radius());
    bandRect.setHeight(0.8*2*mAxis->radius());
    bandRect.moveCenter(mAxis->center());

    int startAngle = int(-1.0*mAxis->angleRad() * 180.0 / M_PI) * 16;
    int currentAngle = int(-1.0*mAxis->spanAngle()) * 16;

    painter->setPen(pen);
    painter->drawArc(bandRect, startAngle, currentAngle);
}

void GaugeRect::drawBand(QCPPainter *painter)
{
    QPen pen;
    pen.setColor(mCurrentColor);
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(mAxis->radius() / 10.0);
    painter->setBrush(Qt::NoBrush);

    QRectF bandRect;
    bandRect.setWidth(0.8*2*mAxis->radius());
    bandRect.setHeight(0.8*2*mAxis->radius());
    bandRect.moveCenter(mAxis->center());

    int startAngle = int(-1.0*mAxis->angleRad() * 180.0 / M_PI) * 16;
    int currentAngle = int(-1.0*mAxis->coordToAngleRad(mValue)* 180.0 / M_PI) * 16;
    painter->setPen(pen);
    painter->drawArc(bandRect, startAngle, currentAngle - startAngle);
}

void GaugeRect::draw(QCPPainter *painter)
{
    drawBackgroundBand(painter);

    if(mThresholdsVisible) {
        drawColorBands(painter);
    }

    if(mEnabled) {
        drawBand(painter);
        drawNeedle(painter);
    }

    drawCenter(painter);
}

#include "gauge_radial_axis.h"
#include "ui/board/management/color_settings.h"

CircularAxis::CircularAxis(Board *dashboard) :
    QCPLayoutElement(dashboard),
    mAngle(135),
    mAngleRad(mAngle/180.0*M_PI),
    mSpanAngle(270),
    mTickLabels(true),
    mTickLabelFont(mParentPlot->font()),
    mNumberPrecision(6),
    mNumberFormatChar('g'),
    mTicks(true),
    mSubTicks(true),
    mTickLengthIn(10),
    mTickLengthOut(0),
    mSubTickLengthIn(4),
    mSubTickLengthOut(0),
    mRange(0, mSpanAngle),
    mRangeReversed(false),
    mRadius(1),
    mTicker(new QCPAxisTicker)
{
    auto textColor = QApplication::palette().toolTipText().color();
    mBasePen = QPen(textColor, 0, Qt::SolidLine, Qt::RoundCap);

    mTickLabelColor = textColor;
    mBaseColor = textColor;
    mTickPen = QPen(textColor, 0, Qt::SolidLine, Qt::RoundCap);
    mSubTickPen = QPen(textColor, 0, Qt::SolidLine, Qt::RoundCap);

    mLabelPainter = new QCPLabelPainterPrivate(dashboard);

    mTicker->setTickCount(6);
    mTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    setAntialiased(true);
    setLayer(QLatin1String("axes"));
    setMinimumMargins(QMargins(45, 30, 45, 30));

    mLabelPainter->setPadding(5);
    mLabelPainter->setRotation(0);
    mLabelPainter->setAnchorMode(QCPLabelPainterPrivate::amSkewedUpright);
}

void CircularAxis::draw(QCPPainter *painter)
{
//    qDebug() << Q_FUNC_INFO << mRect << painter->clipBoundingRect() << painter->clipRegion().boundingRect();
    painter->setPen(mBasePen);
    QRectF axisRect = mRect;
    axisRect.setWidth(2*mRadius);
    axisRect.setHeight(2*mRadius);
    axisRect.moveCenter(mCenter.toPoint());
    painter->drawArc(axisRect, -1*(mAngle)*16, -mSpanAngle*16);

    // draw subticks:
    if (!mSubTickVector.isEmpty()) {
        painter->setPen(mSubTickPen);
        for (int i=0; i<mSubTickVector.size(); ++i) {
            painter->drawLine(mCenter+mSubTickVectorCosSin.at(i)*(mRadius-mSubTickLengthIn),
                              mCenter+mSubTickVectorCosSin.at(i)*(mRadius+mSubTickLengthOut));
        }
    }

    // draw ticks and labels:
    if (!mTickVector.isEmpty()) {
        mLabelPainter->setAnchorReference(mCenter);
        mLabelPainter->setFont(mTickLabelFont);
        mLabelPainter->setColor(mTickLabelColor);
        painter->setClipRect(mOuterRect);
        const QPen ticksPen = mTickPen;
        painter->setPen(ticksPen);
        for (int i=0; i<mTickVector.size(); ++i) {
            const QPointF outerTick = mCenter+mTickVectorCosSin.at(i)*(mRadius+mTickLengthOut);
            painter->drawLine(mCenter+mTickVectorCosSin.at(i)*(mRadius-mTickLengthIn), outerTick);
            // draw tick labels:
            if (!mTickVectorLabels.isEmpty()) {
                if (i < mTickVectorLabels.count()-1 || (mTickVectorCosSin.at(i)-mTickVectorCosSin.first()).manhattanLength() > 5/180.0*M_PI) // skip last label if it's closer than approx 5 degrees to first
                    mLabelPainter->drawTickLabel(painter, outerTick, mTickVectorLabels.at(i));
            }
        }
    }
}

void CircularAxis::setupTickVectors()
{
    if (!mParentPlot) return;
    if ((!mTicks && !mTickLabels ) || mRange.size() <= 0) return;

    mSubTickVector.clear(); // since we might not pass it to mTicker->generate(), and we don't want old data in there
    mTicker->generate(mRange,
                      mParentPlot->locale(),
                      mNumberFormatChar,
                      mNumberPrecision,
                      mTickVector,
                      mSubTicks ? &mSubTickVector : nullptr,
                      mTickLabels ? &mTickVectorLabels : nullptr);

    // fill cos/sin buffers which will be used by draw() and QCPPolarGrid::draw(), so we don't have to calculate it twice:
    mTickVectorCosSin.resize(mTickVector.size());
    for (int i=0; i<mTickVector.size(); ++i) {
        const double theta = coordToAngleRad(mTickVector.at(i));
        mTickVectorCosSin[i] = QPointF(qCos(theta), qSin(theta));
    }
    mSubTickVectorCosSin.resize(mSubTickVector.size());
    for (int i=0; i<mSubTickVector.size(); ++i) {
        const double theta = coordToAngleRad(mSubTickVector.at(i));
        mSubTickVectorCosSin[i] = QPointF(qCos(theta), qSin(theta));
    }
}

double CircularAxis::angle() const
{
    return mAngle;
}

void CircularAxis::setAngle(double angle)
{
    mAngle = angle;
    mAngleRad = mAngle/180.0*M_PI;
}

double CircularAxis::spanAngle() const
{
    return mSpanAngle;
}

void CircularAxis::setSpanAngle(double spanAngle)
{
    mSpanAngle = spanAngle;
}

QColor CircularAxis::baseColor() const
{
    return mBaseColor;
}

double CircularAxis::angleRad() const
{
    return mAngleRad;
}

QPointF CircularAxis::center() const
{
    return mCenter;
}

double CircularAxis::radius() const
{
    return mRadius;
}

QCPRange CircularAxis::range() const
{
    return mRange;
}

void CircularAxis::setRange(const QCPRange &range)
{
    if (qFuzzyCompare(range.lower,mRange.lower) && qFuzzyCompare(range.upper,mRange.upper))
        return;

    if (!QCPRange::validRange(range)) return;
    mRange = range.sanitizedForLinScale();
}

void CircularAxis::setTickLabelPadding(int padding)
{
    Q_UNUSED(padding)
    //    mLabelPainter.setPadding(padding);
}

void CircularAxis::setRange(double lower, double upper)
{
    if (qFuzzyCompare(lower,mRange.lower) && qFuzzyCompare(upper,mRange.upper))
        return;

    if (!QCPRange::validRange(lower, upper))
        return;

    mRange.lower = lower;
    mRange.upper = upper;
    mRange = mRange.sanitizedForLinScale();
}

void CircularAxis::setRangeLower(double lower)
{
    if (qFuzzyCompare(lower,mRange.lower))
        return;

    mRange.lower = lower;
    mRange = mRange.sanitizedForLinScale();
}

void CircularAxis::setRangeUpper(double upper)
{
    if (qFuzzyCompare(upper,mRange.upper))
        return;

    mRange.upper = upper;
    mRange = mRange.sanitizedForLinScale();
}

void CircularAxis::update(QCPLayoutElement::UpdatePhase phase)
{
    QCPLayoutElement::update(phase);

    switch (phase)
    {
    case upPreparation:
    {
        setupTickVectors();
        break;
    }
    case upLayout:
    {
        mCenter = mRect.center();
        mRadius = 0.5*qMin(qAbs(mRect.width()), qAbs(mRect.height()));
        if (mRadius < 1) mRadius = 1; // prevent cases where radius might become 0 which causes trouble

        break;
    }
    default: break;
    }
}

QPen CircularAxis::subTickPen() const
{
    return mSubTickPen;
}

void CircularAxis::setSubTickPen(const QPen &subTickPen)
{
    mSubTickPen = subTickPen;
}

QPen CircularAxis::tickPen() const
{
    return mTickPen;
}

void CircularAxis::setTickPen(const QPen &tickPen)
{
    mTickPen = tickPen;
}

int CircularAxis::subTickLengthOut() const
{
    return mSubTickLengthOut;
}

void CircularAxis::setSubTickLengthOut(int outside)
{
    if (mSubTickLengthOut != outside)
        mSubTickLengthOut = outside;
}

int CircularAxis::subTickLengthIn() const
{
    return mSubTickLengthIn;
}

void CircularAxis::setSubTickLengthIn(int inside)
{
    if (mSubTickLengthIn != inside)
        mSubTickLengthIn = inside;
}

int CircularAxis::tickLengthOut() const
{
    return mTickLengthOut;
}

void CircularAxis::setTickLengthOut(int outside)
{
    if (mTickLengthOut != outside)
        mTickLengthOut = outside;
}

int CircularAxis::tickLengthIn() const
{
    return mTickLengthIn;
}

void CircularAxis::setTickLengthIn(int inside)
{
    if (mTickLengthIn != inside)
        mTickLengthIn = inside;
}

bool CircularAxis::subTicks() const
{
    return mSubTicks;
}

void CircularAxis::setSubTicks(bool show)
{
    if (mSubTicks != show)
        mSubTicks = show;
}

bool CircularAxis::ticks() const
{
    return mTicks;
}

void CircularAxis::setTicks(bool show)
{
    if (mTicks != show)
        mTicks = show;
}

QColor CircularAxis::tickLabelColor() const
{
    return mTickLabelColor;
}

void CircularAxis::setTickLabelColor(const QColor &tickLabelColor)
{
    mTickLabelColor = tickLabelColor;
}

QFont CircularAxis::tickLabelFont() const
{
    return mTickLabelFont;
}

void CircularAxis::setTickLabelFont(const QFont &tickLabelFont)
{
    mTickLabelFont = tickLabelFont;
}

bool CircularAxis::tickLabels() const
{
    return mTickLabels;
}

void CircularAxis::setTickLabels(bool show)
{
    if (mTickLabels != show) {
        mTickLabels = show;
        if (!mTickLabels)
            mTickVectorLabels.clear();
    }
}

QPen CircularAxis::basePen() const
{
    return mBasePen;
}

void CircularAxis::setBasePen(const QPen &basePen)
{
    mBasePen = basePen;
}



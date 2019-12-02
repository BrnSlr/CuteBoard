#include "circularaxis.h"

QTBCircularAxis::QTBCircularAxis(QTBoard *dashboard) :
    QCPLayoutElement(dashboard),
    mAngle(135),
    mAngleRad(mAngle/180.0*M_PI),
    mSpanAngle(270),
    mBasePen(QPen(dashboard->frontColor(), 1, Qt::SolidLine, Qt::RoundCap)),
    // tick labels:
    //mTickLabelPadding(0), in label painter
    mTickLabels(true),
    //mTickLabelRotation(0), in label painter
    mTickLabelFont(mParentPlot->font()),
    mTickLabelColor(dashboard->frontColor()),
    mBaseColor(dashboard->frontColor()),
    mNumberPrecision(6),
    mNumberFormatChar('g'),
    // ticks and subticks:
    mTicks(true),
    mSubTicks(true),
    mTickLengthIn(6),
    mTickLengthOut(0),
    mSubTickLengthIn(3),
    mSubTickLengthOut(0),
    mTickPen(QPen(dashboard->frontColor(), 1, Qt::SolidLine, Qt::RoundCap)),
    mSubTickPen(QPen(dashboard->frontColor(), 1, Qt::SolidLine, Qt::RoundCap)),
    // scale and range:
    mRange(0, mSpanAngle),
    mRangeReversed(false),
    mRadius(1),
    mTicker(new QCPAxisTicker),
    mLabelPainter(dashboard)
{

    mTicker->setTickCount(6);
    mTicker->setTickStepStrategy(QCPAxisTicker::tssReadability);
    setAntialiased(true);
    setLayer(QLatin1String("axes"));
    setMinimumMargins(QMargins(30, 30, 30, 0));

    mLabelPainter.setPadding(5);
    mLabelPainter.setRotation(0);
    mLabelPainter.setAnchorMode(QCPLabelPainterPrivate::amSkewedUpright);
}

void QTBCircularAxis::draw(QCPPainter *painter)
{
    painter->setPen(mBasePen);
    QRectF axisRect = mRect;
    axisRect.setWidth(2*mRadius);
    axisRect.setHeight(2*mRadius);
    axisRect.moveCenter(mCenter.toPoint());
    painter->drawArc(axisRect, -45*16, 270*16);

    // draw subticks:
    if (!mSubTickVector.isEmpty())
    {
        painter->setPen(mSubTickPen);
        for (int i=0; i<mSubTickVector.size(); ++i)
        {
            painter->drawLine(mCenter+mSubTickVectorCosSin.at(i)*(mRadius-mSubTickLengthIn),
                              mCenter+mSubTickVectorCosSin.at(i)*(mRadius+mSubTickLengthOut));
        }
    }

    // draw ticks and labels:
    if (!mTickVector.isEmpty())
    {
        mLabelPainter.setAnchorReference(mCenter);
        mLabelPainter.setFont(mTickLabelFont);
        mLabelPainter.setColor(mTickLabelColor);
        const QPen ticksPen = mTickPen;
        painter->setPen(ticksPen);
        for (int i=0; i<mTickVector.size(); ++i)
        {
            const QPointF outerTick = mCenter+mTickVectorCosSin.at(i)*(mRadius+mTickLengthOut);
            painter->drawLine(mCenter+mTickVectorCosSin.at(i)*(mRadius-mTickLengthIn), outerTick);
            // draw tick labels:
            if (!mTickVectorLabels.isEmpty())
            {
                if (i < mTickVectorLabels.count()-1 || (mTickVectorCosSin.at(i)-mTickVectorCosSin.first()).manhattanLength() > 5/180.0*M_PI) // skip last label if it's closer than approx 5 degrees to first
                    mLabelPainter.drawTickLabel(painter, outerTick, mTickVectorLabels.at(i));
            }
        }
    }
}

void QTBCircularAxis::setupTickVectors()
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
    for (int i=0; i<mTickVector.size(); ++i)
    {
        const double theta = coordToAngleRad(mTickVector.at(i));
        mTickVectorCosSin[i] = QPointF(qCos(theta), qSin(theta));
    }
    mSubTickVectorCosSin.resize(mSubTickVector.size());
    for (int i=0; i<mSubTickVector.size(); ++i)
    {
        const double theta = coordToAngleRad(mSubTickVector.at(i));
        mSubTickVectorCosSin[i] = QPointF(qCos(theta), qSin(theta));
    }
}

double QTBCircularAxis::spanAngle() const
{
    return mSpanAngle;
}

void QTBCircularAxis::setSpanAngle(double spanAngle)
{
    mSpanAngle = spanAngle;
}

QColor QTBCircularAxis::baseColor() const
{
    return mBaseColor;
}

double QTBCircularAxis::angleRad() const
{
    return mAngleRad;
}

QPointF QTBCircularAxis::center() const
{
    return mCenter;
}

double QTBCircularAxis::radius() const
{
    return mRadius;
}

QCPRange QTBCircularAxis::range() const
{
    return mRange;
}

void QTBCircularAxis::setRange(const QCPRange &range)
{
    if (qFuzzyCompare(range.lower,mRange.lower) && qFuzzyCompare(range.upper,mRange.upper))
        return;

    if (!QCPRange::validRange(range)) return;
    mRange = range.sanitizedForLinScale();
}

void QTBCircularAxis::setTickLabelPadding(int padding)
{
    Q_UNUSED(padding)
    //    mLabelPainter.setPadding(padding);
}

void QTBCircularAxis::setRange(double lower, double upper)
{
    if (qFuzzyCompare(lower,mRange.lower) && qFuzzyCompare(upper,mRange.upper))
        return;

    if (!QCPRange::validRange(lower, upper)) return;
    mRange.lower = lower;
    mRange.upper = upper;
    mRange = mRange.sanitizedForLinScale();
}

void QTBCircularAxis::setRangeLower(double lower)
{
    if (qFuzzyCompare(lower,mRange.lower))
        return;

    mRange.lower = lower;
    mRange = mRange.sanitizedForLinScale();
}

void QTBCircularAxis::setRangeUpper(double upper)
{
    if (qFuzzyCompare(upper,mRange.upper))
        return;

    mRange.upper = upper;
    mRange = mRange.sanitizedForLinScale();
}

void QTBCircularAxis::update(QCPLayoutElement::UpdatePhase phase)
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

QPen QTBCircularAxis::subTickPen() const
{
    return mSubTickPen;
}

void QTBCircularAxis::setSubTickPen(const QPen &subTickPen)
{
    mSubTickPen = subTickPen;
}

QPen QTBCircularAxis::tickPen() const
{
    return mTickPen;
}

void QTBCircularAxis::setTickPen(const QPen &tickPen)
{
    mTickPen = tickPen;
}

int QTBCircularAxis::subTickLengthOut() const
{
    return mSubTickLengthOut;
}

void QTBCircularAxis::setSubTickLengthOut(int outside)
{
    if (mSubTickLengthOut != outside)
    {
        mSubTickLengthOut = outside;
    }
}

int QTBCircularAxis::subTickLengthIn() const
{
    return mSubTickLengthIn;
}

void QTBCircularAxis::setSubTickLengthIn(int inside)
{
    if (mSubTickLengthIn != inside)
    {
        mSubTickLengthIn = inside;
    }
}

int QTBCircularAxis::tickLengthOut() const
{
    return mTickLengthOut;
}

void QTBCircularAxis::setTickLengthOut(int outside)
{
    if (mTickLengthOut != outside)
    {
        mTickLengthOut = outside;
    }
}

int QTBCircularAxis::tickLengthIn() const
{
    return mTickLengthIn;
}

void QTBCircularAxis::setTickLengthIn(int inside)
{
    if (mTickLengthIn != inside)
    {
        mTickLengthIn = inside;
    }
}

bool QTBCircularAxis::subTicks() const
{
    return mSubTicks;
}

void QTBCircularAxis::setSubTicks(bool show)
{
    if (mSubTicks != show)
    {
        mSubTicks = show;
    }
}

bool QTBCircularAxis::ticks() const
{
    return mTicks;
}

void QTBCircularAxis::setTicks(bool show)
{
    if (mTicks != show)
    {
        mTicks = show;
    }
}

QColor QTBCircularAxis::tickLabelColor() const
{
    return mTickLabelColor;
}

void QTBCircularAxis::setTickLabelColor(const QColor &tickLabelColor)
{
    mTickLabelColor = tickLabelColor;
}

QFont QTBCircularAxis::tickLabelFont() const
{
    return mTickLabelFont;
}

void QTBCircularAxis::setTickLabelFont(const QFont &tickLabelFont)
{
    mTickLabelFont = tickLabelFont;
}

bool QTBCircularAxis::tickLabels() const
{
    return mTickLabels;
}

void QTBCircularAxis::setTickLabels(bool show)
{
    if (mTickLabels != show)
    {
        mTickLabels = show;
        if (!mTickLabels)
            mTickVectorLabels.clear();
    }
}

QPen QTBCircularAxis::basePen() const
{
    return mBasePen;
}

void QTBCircularAxis::setBasePen(const QPen &basePen)
{
    mBasePen = basePen;
}



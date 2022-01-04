#ifndef CIRCULARGAUGE_H
#define CIRCULARGAUGE_H

#include "ui/board/board.h"

class CircularAxis : public QCPLayoutElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    CircularAxis(Board *dashboard);

    QPen basePen() const;
    void setBasePen(const QPen &basePen);

    bool tickLabels() const;
    void setTickLabels(bool tickLabels);

    QFont tickLabelFont() const;
    void setTickLabelFont(const QFont &tickLabelFont);

    QColor tickLabelColor() const;
    void setTickLabelColor(const QColor &tickLabelColor);

    bool ticks() const;
    void setTicks(bool show);

    bool subTicks() const;
    void setSubTicks(bool show);

    int tickLengthIn() const;
    void setTickLengthIn(int inside);

    int tickLengthOut() const;
    void setTickLengthOut(int outside);

    int subTickLengthIn() const;
    void setSubTickLengthIn(int inside);

    int subTickLengthOut() const;
    void setSubTickLengthOut(int outside);

    QPen tickPen() const;
    void setTickPen(const QPen &tickPen);

    QPen subTickPen() const;
    void setSubTickPen(const QPen &subTickPen);

    QCPRange range() const;
    void setRange(const QCPRange &range);

    void setTickLabelPadding(int padding);

    void setRange(double lower, double upper);
    void setRangeLower(double lower);
    void setRangeUpper(double upper);

    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;

    double coordToAngleRad(double coord) const {
        if(coord < mRange.lower)
            return mAngleRad;
        if(coord > mRange.upper)
            return mAngleRad+(mRangeReversed ? -(mSpanAngle/360.0)*2.0*M_PI : (mSpanAngle/360.0)*2.0*M_PI);

        return mAngleRad+(coord-mRange.lower)/mRange.size()*(mRangeReversed ? -(mSpanAngle/360.0)*2.0*M_PI : (mSpanAngle/360.0)*2.0*M_PI);
    } // mention in doc that return doesn't wrap

    double radius() const;
    QPointF center() const;
    double angleRad() const;
    QColor baseColor() const;
    double spanAngle() const;

    void setSpanAngle(double spanAngle);

    double angle() const;
    void setAngle(double angle);

protected:
    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
    void setupTickVectors();


    double mAngle;
    double mAngleRad;
    double mSpanAngle;
    QPen mBasePen;
    // tick labels:
    bool mTickLabels;
    QFont mTickLabelFont;
    QColor mTickLabelColor;
    QColor mBaseColor;
    int mNumberPrecision;
    QLatin1Char mNumberFormatChar;
    // ticks and subticks:
    bool mTicks;
    bool mSubTicks;
    int mTickLengthIn, mTickLengthOut, mSubTickLengthIn, mSubTickLengthOut;
    QPen mTickPen;
    QPen mSubTickPen;
    // scale and range:
    QCPRange mRange;
    bool mRangeReversed;

    QPointF mCenter;
    double mRadius;
    QVector<double> mTickVector;
    QVector<QString> mTickVectorLabels;
    QVector<QPointF> mTickVectorCosSin;
    QVector<double> mSubTickVector;
    QVector<QPointF> mSubTickVectorCosSin;
    QSharedPointer<QCPAxisTicker> mTicker;
    QCPLabelPainterPrivate *mLabelPainter;

    friend class QTBGaugeRect;
};

#endif // CIRCULARGAUGE_H

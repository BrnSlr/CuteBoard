#ifndef GRAPH_H
#define GRAPH_H

#include "qcustomplot.h"

class QCP_LIB_DECL GraphColoredSegment
{
public:
    GraphColoredSegment(QCPDataRange dataRange, QPen pen, QBrush brush) :
        mDataRange(dataRange),
        mPen(pen),
        mBrush(brush) {}

    QCPDataRange dataRange() const { return mDataRange; }
    QPen pen() const { return mPen; }
    QBrush brush() const { return mBrush; }

private:
    QCPDataRange mDataRange;
    QPen mPen;
    QBrush mBrush;
};

class QCP_LIB_DECL PlotTimeGraph : public QCPGraph
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum FillStyle { fsZero,
                     fsBottom
                   };
    PlotTimeGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void addColoredSegment(double lowerValue, double upperValue, QPen pen, QBrush brush);
    void clearColoredSegments();

    FillStyle getFillStyle() const;
    void setFillStyle(const FillStyle &fillStyle);

protected:
    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
    virtual void drawFill(QCPPainter *painter, QVector<QPointF> *lines) const Q_DECL_OVERRIDE;
    const QPolygonF getCustomFillPolygon(const QVector<QPointF> *lineData, QCPDataRange segment) const;
    QPointF getCustomFillBasePoint(QPointF matchingDataPoint) const;

private:
    QList<double> mLowerValues;
    QList<double> mUpperValues;
    QList<QPen> mPens;
    QList<QBrush> mBrushs;
    FillStyle mFillStyle;

    inline static bool lessThanSegment(const GraphColoredSegment &a, const GraphColoredSegment &b) { return a.dataRange().begin() < b.dataRange().begin(); }

};

#endif // GRAPH_H

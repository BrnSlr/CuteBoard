#ifndef PLOT_TRACER_H
#define PLOT_TRACER_H

#include "axis_rect.h"

class QCP_LIB_DECL PlotTimeTracer : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit PlotTimeTracer(AxisRect *axisrect);
    ~PlotTimeTracer();

    void initialize();

    bool visible() const;
    QCPAxis::AxisType yAxisType() const;
    QPointF mousePos() const;

    void setVisible(bool visible);
    void setYAxisType(const QCPAxis::AxisType &yAxisType);
    void setMousePos(const QPointF &mousePos);

    void updateItems();
    QColor lineColor() const;
    void setLineColor(const QColor &lineColor);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

    QBrush textBrush() const;
    void setTextBrush(const QBrush &textBrush);

signals:

public slots:

protected:
    AxisRect *mAxisRect;
    QCPItemStraightLine *mVerticalLine;
    QVector<QCPItemTracer*> mTracers;
    QVector<QCPItemLine*> mArrows;
    QVector<QCPItemText*> mTexts;

    QCPItemText* mTimeText;

    QColor mLineColor;
    QColor mTextColor;
    QBrush mTextBrush;

    bool mVisible;
    QCPAxis::AxisType mYAxisType;
    QPointF mMousePos;

    bool mIsInitialized;
};

#endif // PLOT_TRACER_H

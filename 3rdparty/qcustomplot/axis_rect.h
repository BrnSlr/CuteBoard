#ifndef AXISRECT_H
#define AXISRECT_H

#include "qcustomplot.h"

class QCP_LIB_DECL AxisRect : public QCPAxisRect
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    AxisRect(QCustomPlot *plot);
    void clearPlottables();

    void setAxisColor(QColor color);



protected:
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
};

#endif // AXISRECT_H

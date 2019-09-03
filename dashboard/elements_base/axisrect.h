#ifndef AXISRECT_H
#define AXISRECT_H

#include "dashboard/dashboard.h"

class QTBAxisRect : public QCPAxisRect
{
    Q_OBJECT
public:
    QTBAxisRect(QTBoard *dashboard);

    void showAxis(QCPAxis::AxisType at, bool visible);
    void clearPlottables();

    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;

protected:
    bool mBottomAxisVisible;
    bool mLeftAxisVisible;
};

#endif // AXISRECT_H

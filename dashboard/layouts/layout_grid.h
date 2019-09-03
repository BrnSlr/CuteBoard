#ifndef LAYOUTGRID_H
#define LAYOUTGRID_H

#include "dashboard/dashboard.h"

class QTBLayoutGrid : public QCPLayoutGrid
{
    Q_OBJECT
public:
    QTBLayoutGrid();

    virtual void updateLayout() Q_DECL_OVERRIDE;

    bool needUpdate() const;
    void needUpdate(bool needUpdate);

protected:
    QRectF mPreviousRect;
    bool mNeedUpdate;

    friend class QTBSingleDisplay;
    friend class QTBAlarmPanel;
};

#endif // LAYOUTGRID_H

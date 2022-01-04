#ifndef LAYOUTGRID_H
#define LAYOUTGRID_H

#include "ui/board/board.h"

class BOARD_UI_EXPORT LayoutGrid : public QCPLayoutGrid
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    LayoutGrid();

    virtual void updateLayout() Q_DECL_OVERRIDE;

    bool needUpdate() const;
    void needUpdate(bool needUpdate);

protected:
    QRectF mPreviousRect;
    bool mNeedUpdate;

    friend class SingleDisplay;
    friend class NumericalDisplay;
    friend class GaugeHorizontal;
    friend class GaugeVertical;
    friend class AlarmPanel;
    friend class TableValues;
};

#endif // LAYOUTGRID_H

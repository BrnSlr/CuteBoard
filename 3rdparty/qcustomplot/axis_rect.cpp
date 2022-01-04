#include "axis_rect.h"

AxisRect::AxisRect(QCustomPlot* parentPlot) :
    QCPAxisRect(parentPlot)
{
    setMinimumMargins(QMargins(22,10,22,10));
    setMinimumSize(1, 1);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    axis(QCPAxis::atRight)->setVisible(true);
    axis(QCPAxis::atLeft)->setVisible(false);

    axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
    axis(QCPAxis::atRight)->setBasePen(Qt::NoPen);
    axis(QCPAxis::atLeft)->setBasePen(Qt::NoPen);
    axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);
    axis(QCPAxis::atRight)->setTickPen(Qt::NoPen);
    axis(QCPAxis::atLeft)->setTickPen(Qt::NoPen);
    axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);
    axis(QCPAxis::atRight)->setSubTickPen(Qt::NoPen);
    axis(QCPAxis::atLeft)->setSubTickPen(Qt::NoPen);    
    axis(QCPAxis::atBottom)->grid()->setZeroLinePen(Qt::NoPen);
    axis(QCPAxis::atRight)->grid()->setZeroLinePen(Qt::NoPen);
    axis(QCPAxis::atLeft)->grid()->setZeroLinePen(Qt::NoPen);
    axis(QCPAxis::atRight)->ticker()->setTickCount(3);
    axis(QCPAxis::atLeft)->ticker()->setTickCount(3);

    QList<QCPAxis*> zoomableAxis;
    zoomableAxis.append( axis(QCPAxis::atLeft));
    zoomableAxis.append( axis(QCPAxis::atRight));
    zoomableAxis.append( axis(QCPAxis::atBottom));

    setRangeZoomAxes(zoomableAxis);
    setRangeDragAxes(zoomableAxis);
}

void AxisRect::clearPlottables()
{
    QList<QCPAbstractPlottable*> plotabblesList = plottables();
    int c = plotabblesList.size();

    for (int i=c-1; i >= 0; --i)
        mParentPlot->removePlottable(plotabblesList[i]);
}

void AxisRect::setAxisColor(QColor color)
{
    axis(QCPAxis::atBottom)->setTickLabelColor(color);
    axis(QCPAxis::atRight)->setTickLabelColor(color);
    axis(QCPAxis::atLeft)->setTickLabelColor(color);
    axis(QCPAxis::atBottom)->grid()->setPen(QPen(color, 0, Qt::DotLine));
    axis(QCPAxis::atRight)->grid()->setPen(QPen(color, 0, Qt::DotLine));
    axis(QCPAxis::atLeft)->grid()->setPen(QPen(color, 0, Qt::DotLine));
}

void AxisRect::wheelEvent(QWheelEvent *event)
{  
    if( event->modifiers() & Qt::ShiftModifier )
        setRangeZoom(Qt::Vertical);
    else if ( event->modifiers() & Qt::ControlModifier)
        setRangeZoom(Qt::Horizontal);
    else
        setRangeZoom(Qt::Horizontal | Qt::Vertical);

    QCPAxisRect::wheelEvent(event);

}

void AxisRect::mousePressEvent(QMouseEvent *event, const QVariant &details)
{
    if (event->buttons() & Qt::RightButton) {
        event->ignore();
    } else {
        QCPAxisRect::mousePressEvent(event, details);
    }
}

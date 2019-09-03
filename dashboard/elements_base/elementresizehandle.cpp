#include "elementresizehandle.h"

QTBElementResizeHandle::QTBElementResizeHandle(QCustomPlot *dashboard) :
    QCPLayoutElement(dashboard),
    mSize(4),
    mColor(Qt::darkGray)
{

}

void QTBElementResizeHandle::draw(QCPPainter *painter)
{
    QPen pen;
    pen.setColor(mColor);
    pen.setWidth(2);

    painter->setPen(pen);
    painter->drawLine(mOuterRect.topRight(),
                      mOuterRect.bottomRight());
    painter->drawLine(mOuterRect.bottomLeft(),
                      mOuterRect.bottomRight());
}

int QTBElementResizeHandle::size() const
{
    return mSize;
}

void QTBElementResizeHandle::setSize(int size)
{
    mSize = size;
}

void QTBElementResizeHandle::setColor(const QColor &color)
{
    mColor = color;
}

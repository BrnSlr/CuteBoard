#include "elementhighlightedrect.h"

QTBElementHighlightedRect::QTBElementHighlightedRect(QTBoard *dashboard):
    QCPLayoutElement(dashboard),
    mColor(dashboard->frontColor())
{
    setLayer(QLatin1String("overlay"));
}

void QTBElementHighlightedRect::draw(QCPPainter *painter)
{
    mColor.setAlpha(100);
    painter->setBrush( QBrush(mColor));
    painter->setPen(Qt::NoPen);
    painter->drawRect(mOuterRect);
}

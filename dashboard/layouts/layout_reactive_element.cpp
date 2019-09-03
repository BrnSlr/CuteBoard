#include "layout_reactive_element.h"

QTBLayoutReactiveElement::QTBLayoutReactiveElement(QTBoard *dashboard) :
    QCPLayoutElement (dashboard),
    mTransparentBackground(false)
  //    mBackgroundColor(QColor(25,35,45))
{
}

double QTBLayoutReactiveElement::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)

    if (onlySelectable)
        return -1;

    if (QRectF(mOuterRect).contains(pos))
    {
        if (mParentPlot)
            return mParentPlot->selectionTolerance()*0.99;
        else
        {
            qDebug() << Q_FUNC_INFO << "parent plot not defined";
            return -1;
        }
    } else
        return -1;
}

void QTBLayoutReactiveElement::initializeElement(QTBoard *dashboard)
{
    if(!parentPlot())
        initializeParentPlot(dashboard);
    setLayer(QLatin1String("elements_background"));

    mBackgroundColor = dashboard->backColor();
}

void QTBLayoutReactiveElement::draw(QCPPainter *painter)
{
    drawBackground(painter);
}

void QTBLayoutReactiveElement::drawBackground(QCPPainter *painter)
{
    if(!mTransparentBackground) {
        painter->setBrush( QBrush(mBackgroundColor));
        painter->setPen(Qt::NoPen);
        painter->drawRect(mOuterRect);
    }
}

bool QTBLayoutReactiveElement::transparentBackground() const
{
    return mTransparentBackground;
}

void QTBLayoutReactiveElement::setTransparentBackground(bool transparentBackground)
{
    mTransparentBackground = transparentBackground;
}


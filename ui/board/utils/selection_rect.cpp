#include "selection_rect.h"
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QMouseEvent>


SelectionRect::SelectionRect(QWidget *parent) : QWidget(parent),
    mColor(QApplication::palette().link().color()),
    mPenActive(true),
    mBrushActive(false),
    mResizeHandlesActive(false),
    mResizeHandleSize(10)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlag(Qt::WindowTransparentForInput, true);
    setAttribute( Qt::WA_TransparentForMouseEvents );
    setFocusPolicy( Qt::NoFocus );

    mBrushColor = mColor;
    mBrushColor.setAlpha(100);

    mBrush = QBrush(mBrushColor);
}

SelectionRect::~SelectionRect()
{
}

void SelectionRect::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QPen pen;
    pen = QPen(mColor, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);

    if(mPenActive)
        p.setPen(pen);
    else
        p.setPen(Qt::NoPen);
    if(mBrushActive)
        p.setBrush(mBrush);
    else
        p.setBrush(Qt::NoBrush);
    auto r = rect();
    r.setWidth(r.width() - 1);
    r.setHeight(r.height() - 1);
    p.drawRoundedRect(r, 4, 4);

    if(mResizeHandlesActive) {
        p.setPen(QPen(mColor, 6, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.fillPath(mTopLeftPath, mColor);
        p.fillPath(mTopRightPath, mColor);
        p.fillPath(mBottomRightPath, mColor);
        p.fillPath(mBottomLeftPath, mColor);
    }
}

void SelectionRect::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    updateHandles();
}

void SelectionRect::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateHandles();
}

void SelectionRect::updateHandles()
{
    mTopLeftPath.clear();
    auto point = rect().topLeft();
    point.rx() += 1;
    point.ry() += 1;
    mTopLeftPath.moveTo(point);

    point.rx() += mResizeHandleSize;
    mTopLeftPath.lineTo(point);

    point.rx() -= mResizeHandleSize;
    point.ry() += mResizeHandleSize;
    mTopLeftPath.lineTo(point);

    point = rect().topLeft();
    point.rx() += 1;
    point.ry() += 1;
    mTopLeftPath.lineTo(point);

    mTopRightPath.clear();
    point = rect().topRight();
    point.ry() += 1;
    mTopRightPath.moveTo(point);

    point.ry() += mResizeHandleSize;
    mTopRightPath.lineTo(point);

    point.rx() -= mResizeHandleSize;
    point.ry() -= mResizeHandleSize;
    mTopRightPath.lineTo(point);

    point = rect().topRight();
    point.ry() += 1;
    mTopRightPath.lineTo(point);

    mBottomRightPath.clear();
    point = rect().bottomRight();
    mBottomRightPath.moveTo(point);

    point.ry() -= mResizeHandleSize;
    mBottomRightPath.lineTo(point);

    point.rx() -= mResizeHandleSize;
    point.ry() += mResizeHandleSize;
    mBottomRightPath.lineTo(point);

    point = rect().bottomRight();
    mBottomRightPath.lineTo(point);

    mBottomLeftPath.clear();
    point = rect().bottomLeft();
    point.rx() += 1;
    mBottomLeftPath.moveTo(point);

    point.ry() -= mResizeHandleSize;
    mBottomLeftPath.lineTo(point);

    point.rx() += mResizeHandleSize;
    point.ry() += mResizeHandleSize;
    mBottomLeftPath.lineTo(point);

    point = rect().bottomLeft();
    point.rx() += 1;
    mBottomLeftPath.lineTo(point);
}

void SelectionRect::setPenActive(bool penActive)
{
    mPenActive = penActive;
}

void SelectionRect::setResizeHandleSize(int resizeHandleSize)
{
    mResizeHandleSize = resizeHandleSize;
}

void SelectionRect::setResizeHandlesActive(bool resizeHandlesActive)
{
    mResizeHandlesActive = resizeHandlesActive;
}

SelectionRect::HoverHandle SelectionRect::hoverHandle(QPoint globalPos)
{
    if(!mResizeHandlesActive)
        return hhNone;

    if(mTopLeftPath.contains(mapFromGlobal(globalPos)))
        return hhTopLeft;
    if(mTopRightPath.contains(mapFromGlobal(globalPos)))
        return hhTopRight;
    if(mBottomLeftPath.contains(mapFromGlobal(globalPos)))
        return hhBottomLeft;
    if(mBottomRightPath.contains(mapFromGlobal(globalPos)))
        return hhBottomRight;

    return hhNone;
}

void SelectionRect::setBrushActive(bool brushActive)
{
    mBrushActive = brushActive;
}

void SelectionRect::setColor(const QColor &color)
{
    mColor = color;
    mBrushColor = mColor;
    mBrushColor.setAlpha(100);
    mBrush = QBrush(mBrushColor);
}

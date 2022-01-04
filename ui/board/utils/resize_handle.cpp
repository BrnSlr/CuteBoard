#include "resize_handle.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

ResizeHandle::ResizeHandle(QWidget *parent, Position position) : QWidget(parent),
    mPosition(position)
{
    //    setAttribute(Qt::WA_TransparentForMouseEvents);
    //    setAttribute(Qt::WA_NoMousePropagation);
    //    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //    setAttribute(Qt::WA_NoSystemBackground);
    //    setAttribute(Qt::WA_TranslucentBackground);
    //    setAttribute(Qt::WA_PaintOnScreen);
    setWindowFlag(Qt::WindowTransparentForInput, true);
    //    setAttribute( Qt::WA_TransparentForMouseEvents );
    //    setFocusPolicy( Qt::NoFocus );
    //    setAutoFillBackground(true);

//    setGeometry(10,
//                10,
//                10,
//                10);

}

ResizeHandle::~ResizeHandle()
{

}

void ResizeHandle::paintEvent(QPaintEvent *)
{
    qDebug() << Q_FUNC_INFO;
    if(parentWidget()) {
        QPainter p(this);
        p.setPen(QPen(QApplication::palette().highlight().color(), 6, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        QPainterPath path;

        switch(mPosition) {
        case pTopLeft:
            setGeometry(int(parentWidget()->geometry().topLeft().x()) + 1,
                        int(parentWidget()->geometry().topLeft().y()) + 1,
                        10,
                        10);

            path.moveTo(rect().topLeft());
            path.lineTo(rect().topRight());
            path.lineTo(rect().bottomLeft());
            path.lineTo(rect().topLeft());
            p.fillPath(path, QApplication::palette().highlight());

            break;
        case pTopRight:
            setGeometry(int(parentWidget()->geometry().topRight().x()) - 11,
                        int(parentWidget()->geometry().topRight().y()) + 1,
                        10,
                        10);

            path.moveTo(rect().topRight());
            path.lineTo(rect().bottomRight());
            path.lineTo(rect().topLeft());
            path.lineTo(rect().topRight());
            p.fillPath(path, QApplication::palette().highlight());
            break;
        case pBottomLeft:
            setGeometry(int(parentWidget()->geometry().bottomLeft().x())  + 1,
                        int(parentWidget()->geometry().bottomLeft().y())  - 1,
                        10,
                        10);

            path.moveTo(rect().bottomLeft());
            path.lineTo(rect().bottomRight());
            path.lineTo(rect().topLeft());
            path.lineTo(rect().bottomLeft());
            p.fillPath(path, QApplication::palette().highlight());
            break;
        case pBottomRight:
            setGeometry(int(parentWidget()->geometry().bottomRight().x()) - 11,
                        int(parentWidget()->geometry().bottomRight().y()) - 11,
                        10,
                        10);

            path.moveTo(rect().bottomRight());
            path.lineTo(rect().bottomLeft());
            path.lineTo(rect().topRight());
            path.lineTo(rect().bottomRight());
            p.fillPath(path, QApplication::palette().highlight());

            break;
        }


    }
}

void ResizeHandle::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
}

void ResizeHandle::enterEvent(QEvent *event)
{
    switch(mPosition) {
    case pTopLeft:
    case pBottomRight:
        setCursor(QCursor(Qt::SizeFDiagCursor));
        break;
    case pTopRight:
    case pBottomLeft:
        setCursor(QCursor(Qt::SizeBDiagCursor));
        break;
    }

    QWidget::enterEvent(event);
}

void ResizeHandle::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void ResizeHandle::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

void ResizeHandle::dragMoveEvent(QDragMoveEvent *event)
{
    QWidget::dragMoveEvent(event);
}

void ResizeHandle::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
}


#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent)
    : QLabel(parent)
{

}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    event->accept();
}

void ClickableLabel::leaveEvent(QEvent *event)
{
    QFont f = font();
    f.setUnderline(false);
    setFont(f);
    setCursor(QCursor(Qt::ArrowCursor));
    event->accept();
}

void ClickableLabel::enterEvent(QEvent *event)
{
    QFont f = font();
    f.setUnderline(true);
    setFont(f);
    setCursor(QCursor(Qt::PointingHandCursor));
    event->accept();
}

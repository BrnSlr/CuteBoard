#include "calendar_widget.h"
#include <QPainter>

CalendarWidget::CalendarWidget(QWidget *parent) : QCalendarWidget(parent)
{

}

void CalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    if(!mValidDates.contains(date)) {
        painter->save();

        auto font = painter->font();
        font.setItalic(true);
        painter->setFont(font);
        auto color = palette().color(QPalette::Disabled, QPalette::ButtonText);
        painter->setPen(color);
        painter->drawText(rect, Qt::TextSingleLine | Qt::AlignCenter, QString::number(date.day()));

        painter->restore();
    } else {
        QCalendarWidget::paintCell(painter, rect, date);
    }
}

void CalendarWidget::addValidDate(QDate date)
{
    if(!mValidDates.contains(date))
        mValidDates.append(date);
}

void CalendarWidget::enterEvent(QEvent *event)
{
    QCalendarWidget::enterEvent(event);
}

#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QCalendarWidget>

class CalendarWidget : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget *parent = nullptr);

    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const override;
    void addValidDate(QDate date);

signals:

public slots:

protected:
    void enterEvent(QEvent *event) override;

private:
    QVector<QDate> mValidDates;
};

#endif // CALENDARWIDGET_H

#ifndef ALARMSLISTWIDGET_H
#define ALARMSLISTWIDGET_H

#include <QListWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include "log4qt/logger.h"

class AlarmsListWidget : public QListWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    AlarmsListWidget(QWidget *parent = nullptr);

private slots:
    void contextMenuRequested(const QPoint& pos);
signals:
    void editAlarm(QString label);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
};

#endif // ALARMSLISTWIDGET_H

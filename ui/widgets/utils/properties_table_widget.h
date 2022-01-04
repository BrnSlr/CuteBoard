#ifndef PROPERTIESTREEWIDGET_H
#define PROPERTIESTREEWIDGET_H

#include <QTreeWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include "log4qt/logger.h"

class PropertiesTableWidget : public QTreeWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    PropertiesTableWidget(QWidget *parent = nullptr);

private slots:
    void contextMenuRequested(const QPoint& pos);
signals:
    void editProperties(QString label, QString descr);
    void duplicateProperties(QString label, QString descr);
    void distributeProperties(QString label, QString descr);
    void editAlarm(QString label);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
};

#endif

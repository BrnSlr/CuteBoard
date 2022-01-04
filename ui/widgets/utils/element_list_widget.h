#ifndef ELEMENTLISTWIDGET_H
#define ELEMENTLISTWIDGET_H

#include <QListWidget>
#include <QTreeWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include "log4qt/logger.h"

class ElementListWidget : public QTreeWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    ElementListWidget(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

    QPixmap mDraggedPixmap;
};

#endif

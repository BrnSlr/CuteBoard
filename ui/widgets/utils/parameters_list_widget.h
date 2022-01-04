#ifndef PARAMETERSLISTWIDGET_H
#define PARAMETERSLISTWIDGET_H

#include <QTreeWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include "log4qt/logger.h"

class ParametersListWidget : public QTreeWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    ParametersListWidget(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
};

#endif // PARAMETERSLISTWIDGET_H

#ifndef PARAMETERSLISTWIDGET_H
#define PARAMETERSLISTWIDGET_H

#include <QTreeWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

class ParametersListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ParametersListWidget(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
};

#endif // PARAMETERSLISTWIDGET_H

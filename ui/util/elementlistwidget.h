#ifndef ELEMENTLISTWIDGET_H
#define ELEMENTLISTWIDGET_H

#include <QListWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

class ElementListWidget : public QListWidget
{
    Q_OBJECT
public:
    ElementListWidget(QWidget *parent = nullptr);


protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

    QPixmap mDraggedPixmap;
};

#endif

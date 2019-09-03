#include "elementlistwidget.h"
#include "dashboard/dashboard.h"

ElementListWidget::ElementListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setIconSize(QSize(48,48));

    mDraggedPixmap = QPixmap(128,72);
    mDraggedPixmap.fill(QColor(25,35,45));
}

void ElementListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::elementMimeType()))
        event->accept();
    else
        event->ignore();
}

void ElementListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::elementMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ElementListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QListWidgetItem*> list = selectedItems();
    if(list.count() > 0) {
        QListWidgetItem* item = list.at(0);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(QTBoard::elementMimeType(),item->text().toUtf8());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(mDraggedPixmap);
        drag->exec();
    }
}

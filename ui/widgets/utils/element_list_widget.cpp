#include "element_list_widget.h"
#include "ui/board/board.h"

ElementListWidget::ElementListWidget(QWidget *parent) : QTreeWidget(parent)
{
    setDragEnabled(true);
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setIconSize(QSize(48,48));

    setHeaderHidden(true);

    mDraggedPixmap = QPixmap(128,72);
    mDraggedPixmap.fill(QApplication::palette().mid().color());
}

void ElementListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(Board::elementNameMimeType()))
        event->accept();
    else
        event->ignore();
}

void ElementListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(Board::elementNameMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ElementListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QTreeWidgetItem*> list = selectedItems();
    if(list.count() > 0) {
        QTreeWidgetItem* item = list.at(0);
        if(item->parent()) {
            QMimeData *mimeData = new QMimeData;
            mimeData->setData(Board::elementNameMimeType(),item->text(0).toUtf8());

            mDraggedPixmap = QPixmap(128,72);
            mDraggedPixmap.fill(QApplication::palette().mid().color());
            QPainter p(&mDraggedPixmap);
            p.drawPixmap(51,24, item->icon(0).pixmap(25,25));
            p.end();
            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(mDraggedPixmap);
            drag->exec();
        }
    }
}

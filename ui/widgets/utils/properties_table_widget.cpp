#include "properties_table_widget.h"
#include "ui/board/board.h"

PropertiesTableWidget::PropertiesTableWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    setDragEnabled(true);
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            SLOT(contextMenuRequested(const QPoint&)));
}

void PropertiesTableWidget::contextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* selItem = itemAt(pos);

    if (selItem) {
        int count = 0;
        auto tmpItem = selItem;
        while (tmpItem->parent()) {
            count ++;
            tmpItem = tmpItem->parent();
        }
        if(count == 2) {
            QTreeWidgetItem *parentItem = selItem->parent();
            if(parentItem) {

                QString desc = selItem->text(0);
                QString label = parentItem->text(0);

                QMenu menu(this);
                menu.addAction("Edit");
                menu.addAction("Duplicate");
//                menu.addAction("Distribute to all pages");
                QPoint pt(pos);
                QAction *action = menu.exec(mapToGlobal(pt));
                if(action) {
                    if(action->text() == "Edit") {
                        emit editProperties(label, desc);
                    } else if(action->text() == "Duplicate") {
                        emit duplicateProperties(label, desc);
                    } else if(action->text() == "Distribute to all pages") {
                        emit distributeProperties(label, desc);
                    }
                }
            }
        } else if(count == 1) {
            if(selItem->child(0))
                return;
            QString label = selItem->text(0);

            QMenu menu(this);
            menu.addAction("Edit");
            QPoint pt(pos);
            QAction *action = menu.exec(mapToGlobal(pt));
            if(action) {
                if(action->text() == "Edit") {
                    emit editAlarm(label);
                }
            }
        }
    }
}

void PropertiesTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(Board::propertyNameMimeType()) ||
            event->mimeData()->hasFormat(Board::alarmConfigMimeType()))
        event->accept();
    else
        event->ignore();
}

void PropertiesTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(Board::propertyNameMimeType()) ||
            event->mimeData()->hasFormat(Board::alarmConfigMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void PropertiesTableWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QTreeWidgetItem*> list = selectedItems();
    if(list.count() > 0 ) {

        QTreeWidgetItem *item = list.at(0);

        int count = 0;
        auto tmpItem = item;
        while (tmpItem->parent()) {
            count ++;
            tmpItem = tmpItem->parent();
        }
        if(count == 2) {
            QTreeWidgetItem *parentItem = item->parent();
            if(parentItem) {

                QString desc = item->text(0);
                QString label = parentItem->text(0);

                QByteArray byteArray;
                QDataStream stream(&byteArray, QIODevice::WriteOnly);
                stream << label << desc;
                QMimeData *mimeData = new QMimeData;
                mimeData->setData(Board::propertyNameMimeType(),byteArray);
                QDrag *drag = new QDrag(this);
                drag->setMimeData(mimeData);
                drag->exec();
            }
        } else if(count == 1) {
            if(item->child(0))
                return;
            QMimeData *mimeData = new QMimeData;
            mimeData->setData(Board::alarmConfigMimeType(),item->text(0).toUtf8());
            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->exec();
        }
    }
}

#include "alarmslistwidget.h"
#include "dashboard/dashboard.h"

AlarmsListWidget::AlarmsListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
}

void AlarmsListWidget::contextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* selItem = itemAt(pos);

    if (selItem) {
        QString label = selItem->text();

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

void AlarmsListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::alarmConfigMimeType()))
        event->accept();
    else
        event->ignore();
}

void AlarmsListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::alarmConfigMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void AlarmsListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QListWidgetItem*> list = selectedItems();
    if(list.count() > 0) {
        QListWidgetItem* item = list.at(0);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(QTBoard::alarmConfigMimeType(),item->text().toUtf8());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec();
    }
}

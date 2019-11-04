#include "propertiestablewidget.h"
#include "dashboard/dashboard.h"

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
        QTreeWidgetItem *parentItem = selItem->parent();
        if(parentItem) {

            QString desc = selItem->text(0);
            QString label = parentItem->text(0);

            QMenu menu(this);
            menu.addAction("Edit");
            QPoint pt(pos);
            QAction *action = menu.exec(mapToGlobal(pt));
            if(action) {
                if(action->text() == "Edit") {
                    emit editProperties(label, desc);
                }
            }
        }
    }
}

void PropertiesTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::parameterConfigMimeType()))
        event->accept();
    else
        event->ignore();
}

void PropertiesTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::parameterConfigMimeType())) {
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
        QTreeWidgetItem *parentItem = item->parent();
        if(parentItem) {

            QString desc = item->text(0);
            QString label = parentItem->text(0);

            QByteArray byteArray;
            QDataStream stream(&byteArray, QIODevice::WriteOnly);
            stream << label << desc;
            QMimeData *mimeData = new QMimeData;
            mimeData->setData(QTBoard::parameterConfigMimeType(),byteArray);
            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->exec();
        }
    }
}

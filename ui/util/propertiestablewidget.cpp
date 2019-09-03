#include "propertiestablewidget.h"
#include "dashboard/dashboard.h"

PropertiesTableWidget::PropertiesTableWidget(QWidget *parent) :
    QTableWidget(parent)
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
    QTableWidgetItem* selItem = itemAt(pos);

    if (selItem) {
        QString label = item(selItem->row(),0)->text();
        QString descr = item(selItem->row(),1)->text();

        QMenu menu(this);
        menu.addAction("Edit");
        QPoint pt(pos);
        QAction *action = menu.exec(mapToGlobal(pt));
        if(action) {
            if(action->text() == "Edit") {
                emit editProperties(label, descr);
            }
        }
    }
}

void PropertiesTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::parameterSettingsMimeType()))
        event->accept();
    else
        event->ignore();
}

void PropertiesTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::parameterSettingsMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void PropertiesTableWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QTableWidgetItem*> list = selectedItems();
    if(list.count() == 2) {

        QString label = list.at(0)->text();
        QString desc = list.at(1)->text();

        QByteArray byteArray;
        QDataStream stream(&byteArray, QIODevice::WriteOnly);
        stream << label << desc;
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(QTBoard::parameterSettingsMimeType(),byteArray);
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec();
    }
}

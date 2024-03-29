#include "parameters_list_widget.h"
#include "ui/board/board.h"

ParametersListWidget::ParametersListWidget(QWidget *parent) : QTreeWidget(parent)
{
    setDragEnabled(true);
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
}

void ParametersListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(Board::parameterNameMimeType()))
        event->accept();
    else
        event->ignore();
}

void ParametersListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(Board::parameterNameMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ParametersListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QTreeWidgetItem*> list = selectedItems();
    if(list.count() > 0) {

        QByteArray data;
        QStringList listLabels;
        for(auto item:list)
            if(!item->data(0, Qt::UserRole).toString().isEmpty())
                listLabels.append(item->text(0));
        QDataStream dataStreamWrite(&data, QIODevice::WriteOnly);
        dataStreamWrite << listLabels;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(Board::parameterNameMimeType(),data);
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec();
    }
}

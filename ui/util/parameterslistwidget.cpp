#include "parameterslistwidget.h"
#include "dashboard/dashboard.h"

ParametersListWidget::ParametersListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::SingleSelection );
}

void ParametersListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::dataParameterMimeType()))
        event->accept();
    else
        event->ignore();
}

void ParametersListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(QTBoard::dataParameterMimeType())) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ParametersListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)
    QList<QListWidgetItem*> list = selectedItems();
    if(list.count() > 0) {
        QListWidgetItem* item = list.at(0);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(QTBoard::dataParameterMimeType(),item->text().toUtf8());
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec();
    }
}

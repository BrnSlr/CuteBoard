#include "elementpickerwidget.h"
#include "ui_elementpickerwidget.h"

ElementPickerWidget::ElementPickerWidget(QWidget *parent, QTBDashboardElement::ElementType type) :
    QWidget(parent),
    ui(new Ui::ElementPickerWidget)
{
    ui->setupUi(this);

    QStringList elements = ElementFactory::Instance()->getElementsName();

    for (int i=0; i< elements.count(); i++)
    {
        if(ElementFactory::Instance()->elementType(elements.at(i)) == type
                || type == QTBDashboardElement::etUnknown ||
                (type == QTBDashboardElement::etSingleParam && ElementFactory::Instance()->elementType(elements.at(i)) == QTBDashboardElement::etMultiParam)) {
            QListWidgetItem *item = new QListWidgetItem(elements.at(i),ui->listWidget);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

            if(!ElementFactory::Instance()->elementIconPath(elements.at(i)).isEmpty())
                item->setIcon(QIcon(QPixmap(ElementFactory::Instance()->elementIconPath(elements.at(i))).scaled(25, 25, Qt::KeepAspectRatio)));
            item->setSizeHint (QSize(32,32));
        }
    }

    ui->listWidget->sortItems();
}

ElementPickerWidget::~ElementPickerWidget()
{
    delete ui;
}

QString ElementPickerWidget::selectedElement()
{
    QString name;
    if(ui->listWidget->selectedItems().count() > 0)
        name = ui->listWidget->selectedItems().at(0)->text();

    return name;
}


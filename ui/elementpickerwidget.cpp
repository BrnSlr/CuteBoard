#include "elementpickerwidget.h"
#include "ui_elementpickerwidget.h"

ElementPickerWidget::ElementPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ElementPickerWidget)
{
    ui->setupUi(this);

    QStringList elements = ElementFactory::Instance()->getElementsName();

    for (int i=0; i< elements.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(elements.at(i),ui->listWidget);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        if(!ElementFactory::Instance()->elementIconPath(elements.at(i)).isEmpty())
            item->setIcon(QIcon(QPixmap(ElementFactory::Instance()->elementIconPath(elements.at(i))).scaled(25, 25, Qt::KeepAspectRatio)));
        item->setSizeHint (QSize(32,32));
    }

    ui->listWidget->sortItems();
}

ElementPickerWidget::~ElementPickerWidget()
{
    delete ui;
}


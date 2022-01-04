#include "find_param_widget.h"
#include "ui_find_param_widget.h"

find_param_widget::find_param_widget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::find_param_widget)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &find_param_widget::find);
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, [=](QTreeWidgetItem *item, int ){
        QString pageName;
        if(item->parent()) {
            pageName = item->parent()->text(0);
        } else {
            pageName = item->text(0);
        }
        if(!pageName.isEmpty())
            emit pageRequested(pageName);
    });
}

find_param_widget::~find_param_widget()
{
    delete ui;
}

void find_param_widget::setProject(const QSharedPointer<Project> &project)
{
    mProject = project;
    find();
}

void find_param_widget::find()
{
    auto paramName = ui->lineEdit->text();

    ui->treeWidget->clear();

    if(!paramName.isEmpty()) {
        auto results = mProject->findParameter(paramName);

        for(auto result : results) {
            auto pageName = result.page_name;

            auto items = ui->treeWidget->findItems(pageName, Qt::MatchExactly, 0);
            QTreeWidgetItem *parentItem = nullptr;
            if(items.isEmpty()) {
                parentItem = new QTreeWidgetItem();
                parentItem->setText(0,pageName);
                ui->treeWidget->addTopLevelItem(parentItem);
            } else {
                parentItem = items.at(0);
            }

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0,result.parameter);
//            item->setText(1,result.type_name);
//            item->setText(2,QString::number(result.row));
//            item->setText(3,QString::number(result.column));

            parentItem->addChild(item);
            parentItem->setExpanded(true);
        }
    } else {
        QStringList pagesNames = mProject->pages().keys();

        QCollator collator;
        collator.setNumericMode(true);

        std::sort(pagesNames.begin(),
                  pagesNames.end(),
                  collator);

        for(auto pageName : pagesNames) {
           QTreeWidgetItem *item = new QTreeWidgetItem();
           item->setText(0,pageName);
           ui->treeWidget->addTopLevelItem(item);
        }
    }
}

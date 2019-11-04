#include "parameterpickerwidget.h"
#include "ui_parameterpickerwidget.h"

ParameterPickerWidget::ParameterPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterPickerWidget),
    mDataManager(nullptr)
{
    ui->setupUi(this);
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchString(QString)));
}

ParameterPickerWidget::~ParameterPickerWidget()
{
    delete ui;
}

void ParameterPickerWidget::updateParameterList()
{
    if(!mDataManager.isNull()) {
        ui->listWidget->clear();
        QHash<QString, QString> parameterSourceNames = mDataManager->parameterSourceNames();

        QHash<QString, QString>::iterator i;
        for (i = parameterSourceNames.begin(); i != parameterSourceNames.end(); ++i) {
            QString sourceName = i.value();
            QTreeWidgetItem *parentItem = nullptr;

            if(!sourceName.isEmpty()) {

                QStringList arbo = sourceName.split(QChar('/'));
                for(auto category:arbo) {
                    QList<QTreeWidgetItem *> items;
                    if(parentItem) {
                        for(int k=0; k< parentItem->childCount();k++) {
                            if(category.compare(parentItem->child(k)->text(0)) == 0)
                                items.append(parentItem->child(k));
                        }
                    } else {
                        items = ui->listWidget->findItems(category, Qt::MatchExactly, 0);
                    }
                    if(items.count() > 0) {
                        for(auto item:items) {
                            if(item->childCount() > 0) {
                                parentItem = item;
                                continue;
                            }
                        }
                    } else {
                        QTreeWidgetItem *childItem = new QTreeWidgetItem();
                        childItem->setText(0,category);
                        if(parentItem) {
                            parentItem->addChild(childItem);
                            parentItem = childItem;
                        } else {
                            ui->listWidget->addTopLevelItem(childItem);
                            parentItem = childItem;
                        }
                    }
                }
            }

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0,i.key());
            item->setData(0,Qt::UserRole, i.key());

            if(parentItem) {
                parentItem->addChild(item);
            } else {
                ui->listWidget->addTopLevelItem(item);
            }

        }
        ui->listWidget->sortItems(0, Qt::AscendingOrder);
    }
    searchString(ui->lineEdit->text());
}

void ParameterPickerWidget::setDataManager(const QSharedPointer<QTBDataManager> &dataManager)
{
    mDataManager = dataManager;
}

void ParameterPickerWidget::searchString(const QString& str)
{
    for (int k=0;k<ui->listWidget->topLevelItemCount();k++) {
        bool toHide = true;
        if (str.isEmpty())
            toHide = false;
        if(ui->listWidget->topLevelItem(k)->text(0).contains(str))
            toHide = false;
        ui->listWidget->topLevelItem(k)->setHidden(toHide);
    }
}


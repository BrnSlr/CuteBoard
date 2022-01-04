#include "parameter_picker_widget.h"
#include "ui_parameter_picker_widget.h"
#include "ui/widgets/utils/icon_factory.h"

ParameterPickerWidget::ParameterPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterPickerWidget),
    mDataManager(nullptr),
    mTreeMode(false)
{
    ui->setupUi(this);    

    QIcon action_icon = create_icon(":/icons8_indent_50px.png");
    QAction *action = new QAction(action_icon, QString(""));
    action->setCheckable(true);
    action->setChecked(false);
    ui->toListToolButton->setDefaultAction(action);

    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchString(QString)));
    connect(ui->toListToolButton, SIGNAL(toggled(bool)), this, SLOT(changeListDisplay()));

    ui->listWidget->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->listWidget->header()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->listWidget->header()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->listWidget->header()->setStretchLastSection(false);
}

ParameterPickerWidget::~ParameterPickerWidget()
{
    delete ui;
}

void ParameterPickerWidget::updateParameterList()
{
    if(!mDataManager.isNull()) {
        ui->listWidget->clear();
        QHash<quint32, QSharedPointer<TimeSeries>> parameters;

        if(mDataManager->liveDataEnabled())
            parameters = mDataManager->liveParametersById();
        else
            parameters = mDataManager->playbackParametersById();

        QHash<quint32, QSharedPointer<TimeSeries>>::const_iterator i;

        for (i = parameters.constBegin(); i != parameters.constEnd(); ++i) {
            QString sourceName = i.value()->sourceName();
            QTreeWidgetItem *parentItem = nullptr;

            if(mTreeMode) {
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
            }

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0,i.value()->name());
            item->setData(0,Qt::UserRole, i.value()->name());
            item->setText(1,i.value()->unit());
            if(i.value()->size() > 1)
                item->setText(2,"Vector");
            else
                item->setText(2,"Scalar");

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

void ParameterPickerWidget::changeListDisplay()
{
    mTreeMode = ui->toListToolButton->isChecked();
    updateParameterList();
}

void ParameterPickerWidget::setDataManager(const QSharedPointer<DataManager> &dataManager)
{
    mDataManager = dataManager;

    connect(mDataManager.data(), &DataManager::parametersUpdated,
            this, &ParameterPickerWidget::updateParameterList);
}

void ParameterPickerWidget::searchString(const QString& str)
{
    for (int k=0;k<ui->listWidget->topLevelItemCount();k++) {
        searchAndHide(str, ui->listWidget->topLevelItem(k));
    }
}

bool ParameterPickerWidget::searchAndHide(QString str, QTreeWidgetItem *item)
{
    bool toHide = true;
    if(item->childCount() == 0) {
        if (str.isEmpty())
            toHide = false;
        if(item->text(0).contains(str, Qt::CaseInsensitive))
            toHide = false;
        item->setHidden(toHide);
    } else {
        for (int k=0;k<item->childCount();k++) {
            bool st = searchAndHide(str, item->child(k));
            toHide = toHide && st;
        }
        item->setHidden(toHide);
    }
    return toHide;
}


void ParameterPickerWidget::setProject(const QSharedPointer<Project> &project)
{
    mProject = project;
}

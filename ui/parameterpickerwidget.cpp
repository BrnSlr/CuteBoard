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
        QHash<QString, quint32> paramLabels = mDataManager->parameterLabels();

        QHash<QString, quint32>::iterator i;
        for (i = paramLabels.begin(); i != paramLabels.end(); ++i) {
            QListWidgetItem *item = new QListWidgetItem(i.key());
            item->setData(Qt::UserRole, i.value());
            ui->listWidget->addItem(item);
        }
        ui->listWidget->sortItems();
    }
    searchString(ui->lineEdit->text());
}

void ParameterPickerWidget::setDataManager(const QSharedPointer<QTBDataManager> &dataManager)
{
    mDataManager = dataManager;
}

void ParameterPickerWidget::searchString(const QString& str)
{
    for (int k=0;k<ui->listWidget->count();k++) {
        bool toHide = true;
        if (str.isEmpty())
            toHide = false;
        if(ui->listWidget->item(k)->text().contains(str))
            toHide = false;
        ui->listWidget->item(k)->setHidden(toHide);
    }
}


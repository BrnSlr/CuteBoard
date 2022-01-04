#include "add_parameter_widget.h"
#include "ui_add_parameter_widget.h"

AddParameterWidget::AddParameterWidget(QWidget *parent,
                                       const QSharedPointer<DataManager> &dataManager,
                                       const QSharedPointer<Project> &project) :
    QDialog(parent),
    ui(new Ui::AddParameterWidget)
{
    ui->setupUi(this);

    ui->parameterWidget->setDataManager(dataManager);
    ui->propertiesWidget->setProject(project);
    ui->parameterWidget->setProject(project);

    ui->parameterWidget->updateParameterList();
    ui->propertiesWidget->updateList();
}

AddParameterWidget::~AddParameterWidget()
{
    delete ui;
}


void AddParameterWidget::on_addToolButton_clicked()
{

}

void AddParameterWidget::on_removeToolButton_clicked()
{

}

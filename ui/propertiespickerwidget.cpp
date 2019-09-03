#include "propertiespickerwidget.h"
#include "propertieswidget.h"
#include "propertieseditordialog.h"
#include "ui_propertiespickerwidget.h"
#include <QDialog>
#include <QLayout>
#include <QDialogButtonBox>
#include <utility>

PropertiesPickerWidget::PropertiesPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertiesPickerWidget)
{
    ui->setupUi(this);
    connect(ui->tableWidget, &PropertiesTableWidget::editProperties,
            this, &PropertiesPickerWidget::editProperties);

    connect(ui->tableWidget, &PropertiesTableWidget::cellDoubleClicked,
            this, &PropertiesPickerWidget::itemDoubleClicked);
}

PropertiesPickerWidget::~PropertiesPickerWidget()
{
    delete ui;
}

void PropertiesPickerWidget::setProject(const QSharedPointer<QTBProject> &project)
{
    mProject = project;
}

void PropertiesPickerWidget::on_addButton_clicked()
{
    QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings = QExplicitlySharedDataPointer<QTBParameterConfiguration>(new QTBParameterConfiguration());

    PropertiesEditorDialog dialog(parameterSettings, true);
    int res = dialog.exec();
    if( res == QDialog::Accepted) {
        // save settings
        parameterSettings->saveToFile(mProject->paramSettingsPath());
        mProject->addParameterSettings(parameterSettings);

        // update list
        updateList();
    }

}

void PropertiesPickerWidget::on_reloadButton_clicked()
{
    mProject->loadParametersSettings();
    updateList();
}

void PropertiesPickerWidget::updateList()
{
    ui->tableWidget->setRowCount(0);
    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration>>> parametersSettings = mProject->parametersSettings();

    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration>>>::const_iterator i;
    for (i = parametersSettings.begin(); i != parametersSettings.end(); i++) {
        const QString& label = i.key();
        QStringList descrList = i.value().keys();

        if(!descrList.isEmpty()) {
            foreach (const QString &str, descrList) {
                QTableWidgetItem *itemLabel = new QTableWidgetItem(label);
                QTableWidgetItem *itemConfig = new QTableWidgetItem(str);

                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, itemLabel);
                ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, itemConfig);
            }
        }
    }
}

void PropertiesPickerWidget::itemDoubleClicked(int row, int col )
{
    Q_UNUSED(col)
    QString label = ui->tableWidget->item(row,0)->text();
    QString descr = ui->tableWidget->item(row,1)->text();
    editProperties(label, descr);
}

void PropertiesPickerWidget::editProperties(QString label, QString descr)
{
    QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings = mProject->parameterSettings(std::move(label), std::move(descr));
    if(parameterSettings) {
        PropertiesEditorDialog dialog(parameterSettings, false);
        int res = dialog.exec();
        if( res == QDialog::Accepted) {
            // save settings
            parameterSettings->saveToFile(mProject->paramSettingsPath());
            mProject->addParameterSettings(parameterSettings);

            // update list
            updateList();
        }
    }
}

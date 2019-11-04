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
    connect(ui->treeWidget, &PropertiesTableWidget::editProperties,
            this, &PropertiesPickerWidget::editProperties);

    connect(ui->treeWidget, &PropertiesTableWidget::itemDoubleClicked,
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
        ui->treeWidget->clear();
        QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration>>> parametersSettings = mProject->parametersSettings();

        QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration>>>::const_iterator i;
        for (i = parametersSettings.begin(); i != parametersSettings.end(); i++) {
            const QString& label = i.key();
            QStringList descrList = i.value().keys();

            if(!descrList.isEmpty()) {

                QTreeWidgetItem *parentItem = new QTreeWidgetItem();
                parentItem->setText(0,label);
                parentItem->setData(0,Qt::UserRole, label);

                foreach (const QString &str, descrList) {
                    QTreeWidgetItem *childItem = new QTreeWidgetItem();
                    childItem->setText(0,str);
                    parentItem->addChild(childItem);
                }
                ui->treeWidget->addTopLevelItem(parentItem);
            }
        }
}

void PropertiesPickerWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QTreeWidgetItem *parentItem = item->parent();
    if(parentItem) {
        QString descr = item->text(0);
        QString label = parentItem->text(0);
        editProperties(label, descr);
    }
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

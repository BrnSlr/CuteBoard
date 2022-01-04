#include "properties_picker_widget.h"
#include "properties_widget.h"
#include "alarm_configuration_widget.h"
#include "ui_properties_picker_widget.h"
#include "ui/widgets/utils/icon_factory.h"
#include <QDialog>
#include <QLayout>
#include <QDialogButtonBox>
#include <utility>

PropertiesPickerWidget::PropertiesPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertiesPickerWidget)
{
    ui->setupUi(this);
    QIcon actionReload_icon = create_icon(":/icons8_synchronize_32px.png");
    QIcon actionAdd_icon = create_icon(":/icons8_plus_math_32px_1.png");
    QAction *actionReload = new QAction(actionReload_icon, QString(""));
    QAction *actionAdd = new QAction(actionAdd_icon, QString(""));
    ui->reloadButton->setDefaultAction(actionReload);
    ui->addButton->setDefaultAction(actionAdd);

    connect(ui->treeWidget, &PropertiesTableWidget::editProperties,
            this, &PropertiesPickerWidget::editProperties);

    connect(ui->treeWidget, &PropertiesTableWidget::duplicateProperties,
            this, &PropertiesPickerWidget::duplicateProperties);

    connect(ui->treeWidget, &PropertiesTableWidget::editAlarm,
            this, &PropertiesPickerWidget::editAlarm);

    connect(ui->treeWidget, &PropertiesTableWidget::itemDoubleClicked,
            this, &PropertiesPickerWidget::itemDoubleClicked);

    connect(ui->lineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(searchString(QString)));
}

PropertiesPickerWidget::~PropertiesPickerWidget()
{
    delete ui;
}

void PropertiesPickerWidget::setProject(const QSharedPointer<Project> &project)
{
    mProject = project;

    connect(mProject.data(), &Project::loaded,
            this, &PropertiesPickerWidget::updateList);

    connect(mProject.data(), &Project::updated,
            this, &PropertiesPickerWidget::updateList);
}

void PropertiesPickerWidget::on_addButton_clicked()
{
    QStringList list;
    list << "Parameter" << "Alarm";

    bool ok;
    QString item = QInputDialog::getItem(this, "New property", "Select type :", list, 0, false, &ok);

    if(ok) {
        if(item == QString("Parameter")) {
            QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings = QExplicitlySharedDataPointer<ParameterConfiguration>(new ParameterConfiguration());

            QDialog dial;
            dial.setWindowTitle("New Parameter Configuration");
            QVBoxLayout layout;
            PropertiesWidget w(&dial);
            w.setEditionMode(PropertiesWidget::emCreation);
            w.setPropertiesMode(ParameterConfiguration::cmFull);
            w.updateUi(parameterSettings);

            QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                       | QDialogButtonBox::Cancel);
            layout.addWidget(&w);
            layout.addWidget(&buttonBox);
            dial.setLayout(&layout);


            connect(&buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);
            connect(&buttonBox, &QDialogButtonBox::accepted, [&dial, &w, parameterSettings]() {

                w.updateParameterSettings(parameterSettings);
                if(!parameterSettings->label().isEmpty() && !parameterSettings->description().isEmpty()) {
                    dial.accept();
                } else {
                    QMessageBox::warning(&dial, "Properties", "Error : incomplete parameter (label/descr)");
                    return;
                }
            });

            int res = dial.exec();
            if( res == QDialog::Accepted) {
                // save settings
                parameterSettings->saveToFile(mProject->paramSettingsPath());
                mProject->addParameterSettings(parameterSettings);
            }
        } else {
            QExplicitlySharedDataPointer<Alarms> alarmConfig = QExplicitlySharedDataPointer<Alarms>(new Alarms());

            QDialog dial;
            dial.setWindowTitle("New Alarm Configuration");
            QVBoxLayout *lay = new QVBoxLayout();
            AlarmConfigurationWidget *w = new AlarmConfigurationWidget();
            w->setEditionMode(AlarmConfigurationWidget::emCreation);
            auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                  | QDialogButtonBox::Cancel);

            connect(buttonBox, &QDialogButtonBox::accepted, [&dial, w](){
                if(!w->getUiName().isEmpty())
                    dial.accept();
                else
                    QMessageBox::warning(w, "Alarm Creation", "Error : empty name");

            });
            connect(buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));
            w->setAlarmConfig(alarmConfig);
            w->updateUi();
            lay->addWidget(w);
            lay->addWidget(buttonBox);
            dial.setLayout(lay);
            int res = dial.exec();
            if( res == QDialog::Accepted) {
                // save settings
                w->updateConfig();
                alarmConfig->saveToFile(mProject->alarmsConfigPath());
                mProject->addAlarmConfiguration(alarmConfig);
            }
        }
    }

}

void PropertiesPickerWidget::on_reloadButton_clicked()
{
    mProject->loadAlarmsConfigurations();
    mProject->loadParametersSettings();
    ui->lineEdit->clear();
    updateList();
}

void PropertiesPickerWidget::updateList()
{
    QMap<QString, bool> expandedItems;
    QTreeWidgetItemIterator it(ui->treeWidget);
    while(*it) {
        expandedItems.insert((*it)->data(0, Qt::UserRole).toString(), (*it)->isExpanded());
        ++it;
    }

    ui->treeWidget->clear();

    QTreeWidgetItem *rootParamItem = new QTreeWidgetItem();
    rootParamItem->setText(0,"Parameters");

    QString id = QString("0_Parameters");
    rootParamItem->setData(0,Qt::UserRole, id);
    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<ParameterConfiguration>>> parametersSettings = mProject->parametersSettings();

    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<ParameterConfiguration>>>::const_iterator i;
    for (i = parametersSettings.begin(); i != parametersSettings.end(); i++) {
        const QString& label = i.key();
        QStringList descrList = i.value().keys();

        if(!descrList.isEmpty()) {

            QTreeWidgetItem *parentItem = new QTreeWidgetItem();
            parentItem->setText(0,label);
            id = QString("1_P_%1").arg(label);
            parentItem->setData(0,Qt::UserRole, id);
            rootParamItem->addChild(parentItem);

            foreach (const QString &str, descrList) {
                QTreeWidgetItem *childItem = new QTreeWidgetItem();
                childItem->setText(0,str);
                id = QString("2_P_%1_%2").arg(label).arg(str);
                childItem->setData(0,Qt::UserRole, id);
                parentItem->addChild(childItem);
            }

        }
    }
    ui->treeWidget->addTopLevelItem(rootParamItem);


    QTreeWidgetItem *rootAlarmItem = new QTreeWidgetItem();
    rootAlarmItem->setText(0,"Alarms");
    id = QString("0_Alarms");
    rootAlarmItem->setData(0,Qt::UserRole, id);

    QMap<QString, QExplicitlySharedDataPointer<Alarms>> alarmsConfig = mProject->alarmsConfigurations();
    QStringList listAlarms = alarmsConfig.keys();
    listAlarms.sort();

    foreach(QString alarmName, listAlarms) {
        QTreeWidgetItem *childItem = new QTreeWidgetItem();
        childItem->setText(0,alarmName);
        id = QString("1_A_%1").arg(alarmName);
        childItem->setData(0,Qt::UserRole, id);
        rootAlarmItem->addChild(childItem);
    }

    ui->treeWidget->addTopLevelItem(rootAlarmItem);

    QTreeWidgetItemIterator it2(ui->treeWidget);
    while(*it2) {
        if(expandedItems.contains((*it2)->data(0, Qt::UserRole).toString()))
            (*it2)->setExpanded(expandedItems.value((*it2)->data(0, Qt::UserRole).toString()));
        ++it2;
    }

    searchString(ui->lineEdit->text());
}

void PropertiesPickerWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    int count = 0;

    auto tmpItem = item;
    while (tmpItem->parent()) {
        count ++;
        tmpItem = tmpItem->parent();
    }
    if(count == 2) {
        QTreeWidgetItem *parentItem = item->parent();
        if(parentItem) {
            QString descr = item->text(0);
            QString label = parentItem->text(0);
            editProperties(label, descr);
        }
    } else if(count == 1) {
        if(item->child(0))
            return;
        editAlarm(item->text(0));
    }
}

void PropertiesPickerWidget::editProperties(QString label, QString descr)
{
    QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings = mProject->parameterSettings(std::move(label), std::move(descr));
    if(parameterSettings) {

        QDialog dial;
        dial.setWindowTitle("Edit Parameter Configuration");
        QVBoxLayout layout;
        PropertiesWidget w(&dial);
        w.setEditionMode(PropertiesWidget::emEdition);
        w.setPropertiesMode(ParameterConfiguration::cmFull);
        w.updateUi(parameterSettings);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);
        layout.addWidget(&w);
        layout.addWidget(&buttonBox);
        dial.setLayout(&layout);

        connect(&buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);
        connect(&buttonBox, &QDialogButtonBox::accepted, [&dial, &w, parameterSettings]() {

            w.updateParameterSettings(parameterSettings);
            if(!parameterSettings->label().isEmpty() && !parameterSettings->description().isEmpty()) {
                dial.accept();
            } else {
                QMessageBox::warning(&dial, "Properties", "Error : incomplete parameter (label/descr)");
                return;
            }
        });

        int res = dial.exec();

        if( res == QDialog::Accepted) {
            // save settings
            parameterSettings->saveToFile(mProject->paramSettingsPath());
            mProject->addParameterSettings(parameterSettings);
        }
    }
}

void PropertiesPickerWidget::duplicateProperties(QString label, QString descr)
{
    QDialog dial;
    dial.setWindowTitle("Copy Parameter Configuration");
    QVBoxLayout layout;
    QHBoxLayout dialLayout;

    QVBoxLayout layoutLeft;
    QVBoxLayout layoutRight;

    QLabel labelLabel("New label");
    QLineEdit labelLineEdit(&dial);

    QLabel descrLabel("New description");
    QLineEdit descrLineEdit(&dial);

    layoutLeft.addWidget(&labelLabel);
    layoutLeft.addWidget(&descrLabel);

    layoutRight.addWidget(&labelLineEdit);
    layoutRight.addWidget(&descrLineEdit);

    dialLayout.addLayout(&layoutLeft);
    dialLayout.addLayout(&layoutRight);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, &QDialogButtonBox::accepted,[&dial, &labelLineEdit, &descrLineEdit, this]() {
        if(!labelLineEdit.text().isEmpty() && !descrLineEdit.text().isEmpty()) {
            QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings = mProject->parameterSettings(labelLineEdit.text(),
                                                                                                                 descrLineEdit.text());
            if(parameterSettings) {
                QMessageBox::warning(&dial, "Properties duplication", "Error : already existing");
            } else {
                dial.accept();
            }
        } else {
            QMessageBox::warning(&dial, "Properties duplication", "Error : empty field");
        }
    });
    connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

    layout.addLayout(&dialLayout);
    layout.addWidget(&buttonBox);

    dial.setLayout(&layout);
    int res = dial.exec();
    if( res == QDialog::Accepted) {
        QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings = mProject->parameterSettings(std::move(label), std::move(descr));
        if(parameterSettings) {
            parameterSettings->setLabel(labelLineEdit.text());
            parameterSettings->setDescription(descrLineEdit.text());
            parameterSettings->setUserDefinedLabel(QString());
            parameterSettings->setUserDefinedUnit(QString());
            parameterSettings->setUserLabelEnabled(false);
            parameterSettings->setUserUnitEnabled(false);

            parameterSettings->saveToFile(mProject->paramSettingsPath());
            mProject->addParameterSettings(parameterSettings);
        }
    }
}

void PropertiesPickerWidget::distributeProperties(QString label, QString descr)
{

}

void PropertiesPickerWidget::editAlarm(QString label)
{
    QExplicitlySharedDataPointer<Alarms> alarmConfig = mProject->alarmConfiguration(label);
    if(alarmConfig) {

        QDialog dial;
        dial.setWindowTitle("Edit Alarm Configuration");
        QVBoxLayout *lay = new QVBoxLayout();
        AlarmConfigurationWidget *w = new AlarmConfigurationWidget();
        w->setEditionMode(AlarmConfigurationWidget::emEdition);

        auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                              | QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), &dial, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));

        w->setAlarmConfig(alarmConfig);
        w->updateUi();
        lay->addWidget(w);
        lay->addWidget(buttonBox);
        dial.setLayout(lay);
        int res = dial.exec();
        if( res == QDialog::Accepted) {
            // save settings
            w->updateConfig();
            alarmConfig->saveToFile(mProject->alarmsConfigPath());
            mProject->addAlarmConfiguration(alarmConfig);
        }
    }
}


void PropertiesPickerWidget::searchString(const QString &str)
{
    for (int k=0;k<ui->treeWidget->topLevelItemCount();k++) {
        auto top = ui->treeWidget->topLevelItem(k);
        for(int i=0; i< top->childCount(); i++) {
            auto item = top->child(i);
            bool toHide = true;
            if (str.isEmpty())
                toHide = false;
            if(item->text(0).contains(str, Qt::CaseInsensitive))
                toHide = false;
            item->setHidden(toHide);
        }
    }
}

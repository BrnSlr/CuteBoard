#include "alarmspickerwidget.h"
#include "ui_alarmspickerwidget.h"
#include "alarm_configuration_widget.h"

AlarmsPickerWidget::AlarmsPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmsPickerWidget)
{
    ui->setupUi(this);

    connect(ui->listWidget, &AlarmsListWidget::editAlarm,
            this, &AlarmsPickerWidget::editAlarm);

    connect(ui->listWidget, &AlarmsListWidget::itemDoubleClicked,
            this, &AlarmsPickerWidget::itemDoubleClicked);

    connect(ui->addToolButton, &QToolButton::clicked,
            this, &AlarmsPickerWidget::slot_addButton_clicked);

    connect(ui->reloadToolButton, &QToolButton::clicked,
            this, &AlarmsPickerWidget::slot_reloadButton_clicked);    

}

AlarmsPickerWidget::~AlarmsPickerWidget()
{
    delete ui;
}

void AlarmsPickerWidget::setProject(const QSharedPointer<QTBProject> &project)
{
    mProject = project;
}

void AlarmsPickerWidget::slot_addButton_clicked()
{
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig = QExplicitlySharedDataPointer<QTBAlarmConfiguration>(new QTBAlarmConfiguration());

    QDialog dial;
    QVBoxLayout *lay = new QVBoxLayout();
    QTBAlarmConfigurationWidget *w = new QTBAlarmConfigurationWidget();
    w->setEditionMode(QTBAlarmConfigurationWidget::emCreation);
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

        // update list
        updateList();
    }
}

void AlarmsPickerWidget::slot_reloadButton_clicked()
{
    mProject->loadAlarmsConfigurations();
    updateList();
}

void AlarmsPickerWidget::updateList()
{
    ui->listWidget->clear();

    QMap<QString, QExplicitlySharedDataPointer<QTBAlarmConfiguration>> alarmsConfig = mProject->alarmsConfigurations();
    QStringList listAlarms = alarmsConfig.keys();
    listAlarms.sort();

    foreach(QString alarmName, listAlarms) {
        new QListWidgetItem(alarmName, ui->listWidget);
    }
}

void AlarmsPickerWidget::itemDoubleClicked(QListWidgetItem *item)
{
    editAlarm(item->text());
}

void AlarmsPickerWidget::editAlarm(QString label)
{
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig = mProject->alarmConfiguration(label);
    if(alarmConfig) {

        QDialog dial;
        QVBoxLayout *lay = new QVBoxLayout();
        QTBAlarmConfigurationWidget *w = new QTBAlarmConfigurationWidget();
        w->setEditionMode(QTBAlarmConfigurationWidget::emEdition);

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

            // update list
            updateList();
        }
    }
}

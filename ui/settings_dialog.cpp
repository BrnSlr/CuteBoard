#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include "data/data_source_interface.h"

QTBSettingsDialog::QTBSettingsDialog(QTBoard *board, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBSettingsDialog),
    mBoard(board)
{
    ui->setupUi(this);

    QString appSettingsPath = QApplication::applicationDirPath() + QDir::separator() + QString("Cuteboard.ini");
    QSettings settings(appSettingsPath, QSettings::IniFormat);

    ui->workingDirectoryLineEdit->setText(settings.value(QString("DefaultWorkingDirectory"), QApplication::applicationDirPath()).toString());

    if(mBoard->dataManager()) {
        QMap<QString, DataSourceInterface *> sources = mBoard->dataManager()->dataSources();

        QMapIterator<QString, DataSourceInterface *> Iter(sources);
        while(Iter.hasNext())
        {
            Iter.next();

            DataSourceInterface *dataIf = Iter.value();
            connect(dataIf, &DataSourceInterface::statusChanged, this, &QTBSettingsDialog::updateStatus);
        }
    }

    updateStatus();

    connect(ui->startPushButton, &QPushButton::clicked,
            this, &QTBSettingsDialog::startDataSource);

    connect(ui->stopPushButton, &QPushButton::clicked,
            this, &QTBSettingsDialog::stopDataSource);
}

QTBSettingsDialog::~QTBSettingsDialog()
{
    delete ui;
}

void QTBSettingsDialog::startDataSource()
{
    auto items = ui->tableWidget->selectedItems();

    if(items.count() > 0) {
        int row = items.at(0)->row();

        QString dataSourceName = ui->tableWidget->item(row, 0)->text();

        if(!dataSourceName.isEmpty()) {
            DataSourceInterface *source = mBoard->dataManager()->dataSources().value(dataSourceName);
            if(source) {
                if(source->status() != DataSourceInterface::dssStarted)
                    source->startAcquisition();
            }
        }
    }
}

void QTBSettingsDialog::stopDataSource()
{
    auto items = ui->tableWidget->selectedItems();

    if(items.count() > 0) {
        int row = items.at(0)->row();

        QString dataSourceName = ui->tableWidget->item(row, 0)->text();

        if(!dataSourceName.isEmpty()) {
            qDebug() << dataSourceName;
            DataSourceInterface *source = mBoard->dataManager()->dataSources().value(dataSourceName);
            if(source) {
                qDebug() << "STOP ACQ" <<  dataSourceName;
                source->stopAcquisition();
            }
        }
    }
}

void QTBSettingsDialog::updateStatus()
{
    ui->tableWidget->setRowCount(0);

    if(mBoard->dataManager()) {
        QMap<QString, DataSourceInterface *> sources = mBoard->dataManager()->dataSources();

        QMapIterator<QString, DataSourceInterface *> Iter(sources);
        while(Iter.hasNext())
        {
            Iter.next();
            ui->tableWidget->insertRow(0);
            QTableWidgetItem *itemName = new QTableWidgetItem(Iter.key());
            ui->tableWidget->setItem(0,0,itemName);

            QTableWidgetItem *itemStatus = new QTableWidgetItem(QMetaEnum::fromType<DataSourceInterface::DataSourceStatus>().valueToKey(Iter.value()->status()));
            ui->tableWidget->setItem(0,1,itemStatus);

            QTableWidgetItem *itemPath = new QTableWidgetItem(Iter.value()->currentPath());
            ui->tableWidget->setItem(0,2,itemPath);
        }
    }
}

void QTBSettingsDialog::on_workingDirectoryPushButton_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if(!dirPath.isEmpty()&& !dirPath.isNull()) {
        QString appSettingsPath = QApplication::applicationDirPath() + QDir::separator() + QString("Cuteboard.ini");
        QSettings settings(appSettingsPath, QSettings::IniFormat);

        settings.setValue(QString("DefaultWorkingDirectory"), dirPath);
        ui->workingDirectoryLineEdit->setText(dirPath);
    }
}

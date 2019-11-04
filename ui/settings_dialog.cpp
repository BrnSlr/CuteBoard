#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include "data/data_source_interface.h"

QTBSettingsDialog::QTBSettingsDialog(QTBoard *board, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBSettingsDialog),
    mBoard(board)
{
    ui->setupUi(this);

    if(mBoard->dataManager()) {
        QMap<QString, DataSource *> sources = mBoard->dataManager()->dataSources();

        QMapIterator<QString, DataSource *> Iter(sources);
        while(Iter.hasNext())
        {
            Iter.next();

            DataSource *dataIf = Iter.value();
            connect(dataIf, &DataSource::statusChanged, this, &QTBSettingsDialog::updateStatus);
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
            DataSource *source = mBoard->dataManager()->dataSources().value(dataSourceName);
            if(source) {
                if(source->status() != DataSource::dssRunning)
                    source->start();
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
            DataSource *source = mBoard->dataManager()->dataSources().value(dataSourceName);
            if(source) {
                source->stop();
            }
        }
    }
}

void QTBSettingsDialog::updateStatus()
{
    ui->tableWidget->setRowCount(0);

    if(mBoard->dataManager()) {
        QMap<QString, DataSource *> sources = mBoard->dataManager()->dataSources();

        QMapIterator<QString, DataSource *> Iter(sources);
        while(Iter.hasNext())
        {
            Iter.next();
            ui->tableWidget->insertRow(0);
            QTableWidgetItem *itemName = new QTableWidgetItem(Iter.key());
            ui->tableWidget->setItem(0,0,itemName);

            QTableWidgetItem *itemStatus = new QTableWidgetItem(QMetaEnum::fromType<DataSource::DataSourceStatus>().valueToKey(Iter.value()->status()));
            ui->tableWidget->setItem(0,1,itemStatus);
        }

        ui->tableWidget->resizeColumnToContents(0);
    }
}

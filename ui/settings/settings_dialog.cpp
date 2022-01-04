#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include "data/live_data_source_interface.h"

SettingsDialog::SettingsDialog(Board *board, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    mBoard(board)
{
    ui->setupUi(this);

    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);
    QString wDir = settings.value(QString("WorkingDir")).toString();
    if(!wDir.isEmpty())
        ui->workingDirectoryLineEdit->setText(wDir);

    ui->workingDirectoryLineEdit->setEnabled(false);
    connect(ui->workingDirectoryPushButton, &QPushButton::clicked, this, &SettingsDialog::selectWorkingDir);

    int period = settings.value(QString("RefreshPeriod"),100).toInt();
    ui->spinBox->setValue(period);

    QSettings palsettings(QApplication::applicationDirPath() + QDir::separator() + QString("palettes.ini"),
                       QSettings::IniFormat);

    palsettings.beginGroup("Palettes");
    auto palettes = palsettings.childGroups();
    ui->comboBox->insertItems(0, palettes);
    palsettings.endGroup();
    auto customPaletteName = palsettings.value("DefaultPalette").toString();
    if(customPaletteName.isEmpty()) {
        ui->comboBox->setCurrentIndex(ui->comboBox->count());
    } else {
        if(palettes.contains(customPaletteName))
            ui->comboBox->setCurrentText(customPaletteName);
        else
            ui->comboBox->setCurrentIndex(ui->comboBox->count());
    }

    if(mBoard->dataManager()) {
        auto sources = mBoard->dataManager()->liveDataSources();

        QMapIterator<QString, QSharedPointer<LiveDataSourceInterface>> Iter(sources);
        while(Iter.hasNext())
        {
            Iter.next();

            auto dataIf = Iter.value();
            connect(dataIf.data(), &DataSource::statusChanged,
                    this, &SettingsDialog::updateLiveDataSources);
        }
    }

    if(mBoard->dataManager()->useDataSourceAsReferenceClock())
        ui->clockComboBox->setCurrentIndex(0);
    else
        ui->clockComboBox->setCurrentIndex(1);

    updateLiveDataSources();
    updatePlaybackDataSources();

    connect(ui->startPushButton, &QPushButton::clicked,
            this, &SettingsDialog::startLiveDataSource);

    connect(ui->stopPushButton, &QPushButton::clicked,
            this, &SettingsDialog::stopLiveDataSource);

    connect(ui->clockPushButton, &QPushButton::clicked,
            this, &SettingsDialog::selectDataSourceForReferenceTime);

    connect(ui->clockComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(clockReferenceChanged()));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(colorThemeChanged()));

    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(refreshPeriodChanged()));

    auto buttons = ui->buttonBox->buttons();
    for(auto button : buttons)
        button->setMinimumWidth(80);

    ui->tabWidget->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::startLiveDataSource()
{
    auto items = ui->liveTableWidget->selectedItems();

    if(items.count() > 0) {
        int row = items.at(0)->row();

        QString dataSourceName = ui->liveTableWidget->item(row, 0)->text();

        if(!dataSourceName.isEmpty()) {
            auto source = mBoard->dataManager()->liveDataSources().value(dataSourceName);
            if(source) {
                if(source->status() != DataSource::dssRunning)
                    source->start();
            }
        }
    }
}

void SettingsDialog::stopLiveDataSource()
{
    auto items = ui->liveTableWidget->selectedItems();

    if(items.count() > 0) {
        int row = items.at(0)->row();

        QString dataSourceName = ui->liveTableWidget->item(row, 0)->text();

        if(!dataSourceName.isEmpty()) {
            auto source = mBoard->dataManager()->liveDataSources().value(dataSourceName);
            if(source) {
                source->stop();
            }
        }
    }
}

void SettingsDialog::selectDataSourceForReferenceTime()
{
    auto items = ui->liveTableWidget->selectedItems();

    if(items.count() > 0) {
        int row = items.at(0)->row();

        QString dataSourceName = ui->liveTableWidget->item(row, 0)->text();

        if(!dataSourceName.isEmpty()) {
            auto source = mBoard->dataManager()->liveDataSources().value(dataSourceName);
            if(source) {
                mBoard->dataManager()->setReferenceTimeLiveDataSource(source);

                updateLiveDataSources();
            }
        }
    }
}

void SettingsDialog::clockReferenceChanged()
{
    if(ui->clockComboBox->currentIndex()==0)
        mBoard->dataManager()->setUseDataSourceAsReferenceClock(true);
    else
        mBoard->dataManager()->setUseDataSourceAsReferenceClock(false);

    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);
    settings.setValue(QString("DataRefClock"), mBoard->dataManager()->useDataSourceAsReferenceClock());
}

void SettingsDialog::refreshPeriodChanged()
{
    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);
    settings.setValue("RefreshPeriod", ui->spinBox->value());
}

void SettingsDialog::colorThemeChanged()
{    
    QSettings palsettings(QApplication::applicationDirPath() + QDir::separator() + QString("palettes.ini"),
                       QSettings::IniFormat);
    palsettings.setValue("DefaultPalette", ui->comboBox->currentText());
}

void SettingsDialog::updateLiveDataSources()
{
    ui->liveTableWidget->setRowCount(0);

    if(mBoard->dataManager()) {
        auto sources = mBoard->dataManager()->liveDataSources();

        QMapIterator<QString, QSharedPointer<LiveDataSourceInterface>> it(sources);
        while(it.hasNext())
        {
            it.next();
            ui->liveTableWidget->insertRow(0);
            QTableWidgetItem *itemName = new QTableWidgetItem(it.key());
            ui->liveTableWidget->setItem(0,0,itemName);

            QTableWidgetItem *itemStatus = new QTableWidgetItem(QMetaEnum::fromType<DataSource::DataSourceStatus>().valueToKey(it.value()->status()));
            ui->liveTableWidget->setItem(0,1,itemStatus);

            QTableWidgetItem *itemHasClock = new QTableWidgetItem();
            if(it.value()->isTimeReference())
                itemHasClock->setText("X");
            ui->liveTableWidget->setItem(0,2,itemHasClock);

            QTableWidgetItem *itemRefTime = new QTableWidgetItem();
            if(it.value() == mBoard->dataManager()->referenceTimeLiveDataSource())
                itemRefTime->setText("X");
            ui->liveTableWidget->setItem(0,3,itemRefTime);

        }

        ui->liveTableWidget->resizeColumnToContents(0);
    }
}

void SettingsDialog::updatePlaybackDataSources()
{
    ui->playbackTableWidget->setRowCount(0);

    if(mBoard->dataManager()) {
        auto sources = mBoard->dataManager()->playbackDataSources();

        QMapIterator<QString, QSharedPointer<PlaybackDataSourceInterface>> Iter(sources);
        while(Iter.hasNext())
        {
            Iter.next();
            ui->playbackTableWidget->insertRow(0);
            QTableWidgetItem *itemName = new QTableWidgetItem(Iter.key());
            ui->playbackTableWidget->setItem(0,0,itemName);
        }
    }
}

void SettingsDialog::selectWorkingDir()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        ui->workingDirectoryLineEdit->text(),
                                                        QFileDialog::ShowDirsOnly);

    if(!dirPath.isEmpty()&& !dirPath.isNull()){

        ui->workingDirectoryLineEdit->setText(dirPath);
        QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                           QSettings::IniFormat);
        settings.setValue(QString("WorkingDir"), dirPath);
    }
}

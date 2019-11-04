#include "alarm_configuration_widget.h"
#include "ui_alarm_configuration_widget.h"
#include "ui/util/colorlineedit.h"
#include <QtGlobal>

QTBAlarmConfigurationWidget::QTBAlarmConfigurationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTBAlarmConfigurationWidget)
{
    ui->setupUi(this);
    resizeColumns();

    mModified = false;

    connect(ui->newPushButton,&QPushButton::clicked, this, &QTBAlarmConfigurationWidget::newAlarm );
    connect(ui->downPushButton,&QPushButton::clicked, this, &QTBAlarmConfigurationWidget::moveDown );
    connect(ui->upPushButton,&QPushButton::clicked, this, &QTBAlarmConfigurationWidget::moveUp );
    connect(ui->delPushButton,&QPushButton::clicked, this, &QTBAlarmConfigurationWidget::removeAlarm );
    connect(ui->alarmsTableWidget, &QTableWidget::itemDoubleClicked, this, &QTBAlarmConfigurationWidget::doubleClickedItem);
    connect(ui->connectedCheckBox, &QCheckBox::stateChanged,this, &QTBAlarmConfigurationWidget::stateChanged);
}

QTBAlarmConfigurationWidget::~QTBAlarmConfigurationWidget()
{
    delete ui;
}

void QTBAlarmConfigurationWidget::setEditionMode(const QTBAlarmConfigurationWidget::EditionMode &mode)
{
    mEditionMode = mode;

    switch(mEditionMode) {
    case emCreation:
        ui->connectedCheckBox->setChecked(true);
        ui->connectedCheckBox->setVisible(true);
        ui->connectedCheckBox->setEnabled(false);
        ui->configNameLineEdit->setVisible(true);
        ui->configNameLineEdit->setEnabled(true);
        ui->label->setVisible(true);
        ui->newPushButton->setEnabled(true);
        ui->downPushButton->setEnabled(true);
        ui->delPushButton->setEnabled(true);
        ui->upPushButton->setEnabled(true);
        ui->alarmsTableWidget->setEnabled(true);
        break;
    case emEdition:
        ui->connectedCheckBox->setChecked(true);
        ui->connectedCheckBox->setVisible(true);
        ui->connectedCheckBox->setEnabled(false);
        ui->configNameLineEdit->setVisible(true);
        ui->configNameLineEdit->setEnabled(false);
        ui->label->setVisible(true);
        ui->newPushButton->setEnabled(true);
        ui->downPushButton->setEnabled(true);
        ui->delPushButton->setEnabled(true);
        ui->upPushButton->setEnabled(true);
        ui->alarmsTableWidget->setEnabled(true);
        break;
    case emElementConnected:
        ui->connectedCheckBox->setChecked(true);
        ui->connectedCheckBox->setVisible(true);
        ui->connectedCheckBox->setEnabled(true);
        ui->configNameLineEdit->setVisible(true);
        ui->configNameLineEdit->setEnabled(false);
        ui->label->setVisible(true);
        ui->newPushButton->setEnabled(false);
        ui->downPushButton->setEnabled(false);
        ui->delPushButton->setEnabled(false);
        ui->upPushButton->setEnabled(false);
        ui->alarmsTableWidget->setEnabled(false);
        break;
    case emElementDisconnected:
        ui->connectedCheckBox->setChecked(false);
        ui->connectedCheckBox->setVisible(true);
        ui->connectedCheckBox->setEnabled(true);
        ui->configNameLineEdit->setVisible(true);
        ui->configNameLineEdit->setEnabled(false);
        ui->label->setVisible(true);
        ui->newPushButton->setEnabled(true);
        ui->downPushButton->setEnabled(true);
        ui->delPushButton->setEnabled(true);
        ui->upPushButton->setEnabled(true);
        ui->alarmsTableWidget->setEnabled(true);
        break;
    case emElementStandAlone:
        ui->connectedCheckBox->setChecked(false);
        ui->connectedCheckBox->setVisible(false);
        ui->connectedCheckBox->setEnabled(false);
        ui->configNameLineEdit->setVisible(false);
        ui->configNameLineEdit->setEnabled(false);
        ui->label->setVisible(false);
        ui->newPushButton->setEnabled(true);
        ui->downPushButton->setEnabled(true);
        ui->delPushButton->setEnabled(true);
        ui->upPushButton->setEnabled(true);
        ui->alarmsTableWidget->setEnabled(true);
        break;
    }
}

void QTBAlarmConfigurationWidget::updateConfig()
{
    mAlarmConfig->setName(ui->configNameLineEdit->text());
    mAlarmConfig->clearAlarms();

    for(int i=0; i< ui->alarmsTableWidget->rowCount(); i++) {
        bool active = true;
        if(ui->alarmsTableWidget->item(i, 0)->text() == QString("No"))
            active = false;

        QTBAlarmConfiguration::AlarmFunction func = QTBAlarmConfiguration::afBitSet;
        if(ui->alarmsTableWidget->item(i, 2)->text() == QString("Greater than X"))
            func = QTBAlarmConfiguration::afGreaterThan;
        if(ui->alarmsTableWidget->item(i, 2)->text() == QString("Lower than X"))
            func = QTBAlarmConfiguration::afLowerThan;

        mAlarmConfig->addAlarm(active,
                               ui->alarmsTableWidget->item(i,1)->text(),
                               ui->alarmsTableWidget->item(i,4)->text(),
                               ui->alarmsTableWidget->item(i,5)->text(),
                               func,
                               ui->alarmsTableWidget->item(i,3)->text().toDouble(),
                               ui->alarmsTableWidget->item(i,6)->text());
    }

    mAlarmConfig->setModified(mModified);
}

void QTBAlarmConfigurationWidget::updateUi()
{
    ui->configNameLineEdit->setText(mAlarmConfig->name());
    ui->alarmsTableWidget->setRowCount(0);

    for(int i=0; i< mAlarmConfig->alarmsCount(); i++) {
        ui->alarmsTableWidget->insertRow(ui->alarmsTableWidget->rowCount());

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       0,
                                       new QTableWidgetItem(mAlarmConfig->active().at(i) ? QString("Yes") : QString("No")));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       1,
                                       new QTableWidgetItem(mAlarmConfig->watchedParameters().at(i)));

        switch (mAlarmConfig->functions().at(i)) {
        case QTBAlarmConfiguration::afBitSet:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Bit X Set"));
            break;
        case QTBAlarmConfiguration::afGreaterThan:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Greater than X"));
            break;
        case QTBAlarmConfiguration::afLowerThan:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Lower than X"));
            break;
        }

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       3,
                                       new QTableWidgetItem(QString::number(mAlarmConfig->functionArgs().at(i))));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       4,
                                       new QTableWidgetItem(mAlarmConfig->messages().at(i)));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       5,
                                       new QTableWidgetItem(mAlarmConfig->outputParameters().at(i)));

        auto colorItem = new QTableWidgetItem(mAlarmConfig->colors().at(i).name());
        colorItem->setForeground(mAlarmConfig->colors().at(i));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       6,
                                       colorItem);
    }
}

bool QTBAlarmConfigurationWidget::isConnected()
{
    return ui->connectedCheckBox->isChecked();
}

QString QTBAlarmConfigurationWidget::getUiName()
{
    return ui->configNameLineEdit->text();
}

QExplicitlySharedDataPointer<QTBAlarmConfiguration> QTBAlarmConfigurationWidget::alarmConfig() const
{
    return mAlarmConfig;
}

void QTBAlarmConfigurationWidget::setAlarmConfig(const QExplicitlySharedDataPointer<QTBAlarmConfiguration> &alarmConfig)
{
    mAlarmConfig = alarmConfig;
}

void QTBAlarmConfigurationWidget::newAlarm()
{
    QDialog dial;
    QVBoxLayout dialLayout;

    QCheckBox activeCheck("Active");
    dialLayout.addWidget(&activeCheck);

    QHBoxLayout watchedParamLayout;
    QLabel watchedParamLabel("Watched Parameter");
    QLineEdit watchedParamLineEdit("");
    watchedParamLayout.addWidget(&watchedParamLabel);
    watchedParamLayout.addWidget(&watchedParamLineEdit);
    dialLayout.addLayout(&watchedParamLayout);

    QHBoxLayout functionLayout;
    QLabel functionLabel("Function");
    QComboBox functionCombo;
    functionCombo.addItem("Greater than X");
    functionCombo.addItem("Lower than X");
    functionCombo.addItem("Bit X Set");
    functionLayout.addWidget(&functionLabel);
    functionLayout.addWidget(&functionCombo);
    dialLayout.addLayout(&functionLayout);

    QHBoxLayout functionArgLayout;
    QLabel functionArgLabel("Function Arg Value");
    QDoubleSpinBox argSpinBox;
    argSpinBox.setRange(-9999999999999999.0, 999999999999999999999.0);
    argSpinBox.setDecimals(4);
    functionArgLayout.addWidget(&functionArgLabel);
    functionArgLayout.addWidget(&argSpinBox);
    dialLayout.addLayout(&functionArgLayout);

    QHBoxLayout messageLayout;
    QLabel messageLabel("Displayed Message");
    QLineEdit messageLineEdit("");
    messageLayout.addWidget(&messageLabel);
    messageLayout.addWidget(&messageLineEdit);
    dialLayout.addLayout(&messageLayout);

    connect(&functionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&argSpinBox](int index) {
        if(index == 2)
            argSpinBox.setDecimals(0);
        else
            argSpinBox.setDecimals(4);
    });


    QHBoxLayout displayedParamLayout;
    QLabel didsplayedParamLabel("Displayed Parameter Label");
    QLineEdit displayedParamLineEdit("");
    displayedParamLayout.addWidget(&didsplayedParamLabel);
    displayedParamLayout.addWidget(&displayedParamLineEdit);
    dialLayout.addLayout(&displayedParamLayout);

    QHBoxLayout colorLayout;
    QPushButton colorButton;
    colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));

    ColorLineEdit colorLineEdit;
    colorLineEdit.setMode(QTBColorSettings::cmNoBackground);
    colorLayout.addWidget(&colorButton);
    colorLayout.addWidget(&colorLineEdit);
    dialLayout.addLayout(&colorLayout);

    connect(&colorButton, &QPushButton::clicked, [&colorLineEdit](){
        QColor color = QColorDialog::getColor(colorLineEdit.color());
        if(color.isValid()) {
            colorLineEdit.setColor(color);
        }
    });

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, &QDialogButtonBox::accepted, [&dial, &messageLineEdit, &watchedParamLineEdit](){
        if(!watchedParamLineEdit.text().isEmpty() &&
                !messageLineEdit.text().isEmpty() )
            dial.accept();
        else
            QMessageBox::warning(nullptr, "Alarm Creation", "Error : incomplete configuration");

    });
    connect(&buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));
    dialLayout.addWidget(&buttonBox);


    dial.setLayout(&dialLayout);
    int res = dial.exec();
    if( res == QDialog::Accepted) {
        ui->alarmsTableWidget->insertRow(ui->alarmsTableWidget->rowCount());

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       0,
                                       new QTableWidgetItem(activeCheck.isChecked() ? QString("Yes") : QString("No")));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       1,
                                       new QTableWidgetItem(watchedParamLineEdit.text()));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       2,
                                       new QTableWidgetItem(functionCombo.currentText()));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       3,
                                       new QTableWidgetItem(QString::number(argSpinBox.value())));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       4,
                                       new QTableWidgetItem(messageLineEdit.text()));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       5,
                                       new QTableWidgetItem(displayedParamLineEdit.text()));

        auto colorItem = new QTableWidgetItem(colorLineEdit.color().name());
        colorItem->setForeground(colorLineEdit.color());

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       6,
                                       colorItem);

        mModified = true;
    }
}

void QTBAlarmConfigurationWidget::moveDown()
{
    QList<QTableWidgetItem*> items = ui->alarmsTableWidget->selectedItems();
    if(items.count() > 0) {
        int row = items.at(0)->row();
        if(row < ui->alarmsTableWidget->rowCount() - 1) {

            QList<QTableWidgetItem*> itemsToMoveUp;
            QList<QTableWidgetItem*> itemsToMoveDown;
            for(int i = 0; i< ui->alarmsTableWidget->columnCount();i++) {
                itemsToMoveDown.append(ui->alarmsTableWidget->takeItem(row, i));
                itemsToMoveUp.append(ui->alarmsTableWidget->takeItem(row + 1, i));
            }

            for(int i = 0; i< ui->alarmsTableWidget->columnCount();i++) {
                ui->alarmsTableWidget->setItem(row, i, itemsToMoveUp.at(i));
                ui->alarmsTableWidget->setItem(row + 1, i, itemsToMoveDown.at(i));
            }

            ui->alarmsTableWidget->selectRow(row + 1);
            mModified = true;
        }
    }

}

void QTBAlarmConfigurationWidget::moveUp()
{
    QList<QTableWidgetItem*> items = ui->alarmsTableWidget->selectedItems();
    if(items.count() > 0) {
        int row = items.at(0)->row();
        if(row  > 0 ) {

            QList<QTableWidgetItem*> itemsToMoveUp;
            QList<QTableWidgetItem*> itemsToMoveDown;
            for(int i = 0; i< ui->alarmsTableWidget->columnCount();i++) {
                itemsToMoveDown.append(ui->alarmsTableWidget->takeItem(row - 1, i));
                itemsToMoveUp.append(ui->alarmsTableWidget->takeItem(row, i));
            }

            for(int i = 0; i< ui->alarmsTableWidget->columnCount();i++) {
                ui->alarmsTableWidget->setItem(row -1, i, itemsToMoveUp.at(i));
                ui->alarmsTableWidget->setItem(row, i, itemsToMoveDown.at(i));
            }
        }
        ui->alarmsTableWidget->selectRow(row - 1);
        mModified = true;
    }
}

void QTBAlarmConfigurationWidget::removeAlarm()
{
    QList<QTableWidgetItem*> items = ui->alarmsTableWidget->selectedItems();
    if(items.count() > 0) {
        int row = items.at(0)->row();
        ui->alarmsTableWidget->removeRow(row);
    }
}

void QTBAlarmConfigurationWidget::doubleClickedItem(QTableWidgetItem *item)
{
    if(item) {
        int row = item->row();

        QDialog dial;
        QVBoxLayout dialLayout;

        QCheckBox activeCheck("Active");
        if(ui->alarmsTableWidget->item(row, 0)->text() == QString("Yes"))
            activeCheck.setChecked(true);
        else
            activeCheck.setChecked(false);
        dialLayout.addWidget(&activeCheck);

        QHBoxLayout watchedParamLayout;
        QLabel watchedParamLabel("Watched Parameter");
        QLineEdit watchedParamLineEdit("");
        watchedParamLineEdit.setText(ui->alarmsTableWidget->item(row, 1)->text());
        watchedParamLayout.addWidget(&watchedParamLabel);
        watchedParamLayout.addWidget(&watchedParamLineEdit);
        dialLayout.addLayout(&watchedParamLayout);

        QHBoxLayout functionLayout;
        QLabel functionLabel("Function");
        QComboBox functionCombo;
        functionCombo.addItem("Greater than X");
        functionCombo.addItem("Lower than X");
        functionCombo.addItem("Bit X Set");
        functionCombo.setCurrentText(ui->alarmsTableWidget->item(row, 2)->text());
        functionLayout.addWidget(&functionLabel);
        functionLayout.addWidget(&functionCombo);
        dialLayout.addLayout(&functionLayout);

        QHBoxLayout functionArgLayout;
        QLabel functionArgLabel("Function Arg Value");
        QDoubleSpinBox argSpinBox;
        argSpinBox.setRange(-9999999999999999.0, 999999999999999999999.0);
        argSpinBox.setDecimals(4);
        argSpinBox.setValue(ui->alarmsTableWidget->item(row, 3)->text().toDouble());
        functionArgLayout.addWidget(&functionArgLabel);
        functionArgLayout.addWidget(&argSpinBox);
        dialLayout.addLayout(&functionArgLayout);

        QHBoxLayout messageLayout;
        QLabel messageLabel("Displayed Message");
        QLineEdit messageLineEdit("");
        messageLineEdit.setText(ui->alarmsTableWidget->item(row, 4)->text());
        messageLayout.addWidget(&messageLabel);
        messageLayout.addWidget(&messageLineEdit);
        dialLayout.addLayout(&messageLayout);

        connect(&functionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&argSpinBox](int index) {
            if(index == 2)
                argSpinBox.setDecimals(0);
            else
                argSpinBox.setDecimals(4);
        });


        QHBoxLayout displayedParamLayout;
        QLabel didsplayedParamLabel("Displayed Parameter Label");
        QLineEdit displayedParamLineEdit("");
        displayedParamLineEdit.setText(ui->alarmsTableWidget->item(row, 5)->text());
        displayedParamLayout.addWidget(&didsplayedParamLabel);
        displayedParamLayout.addWidget(&displayedParamLineEdit);
        dialLayout.addLayout(&displayedParamLayout);

        QHBoxLayout colorLayout;
        QPushButton colorButton;
        colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));

        ColorLineEdit colorLineEdit;
        colorLineEdit.setMode(QTBColorSettings::cmNoBackground);
        colorLineEdit.setColor(QColor(ui->alarmsTableWidget->item(row, 6)->text()));
        colorLayout.addWidget(&colorButton);
        colorLayout.addWidget(&colorLineEdit);
        dialLayout.addLayout(&colorLayout);

        connect(&colorButton, &QPushButton::clicked, [&colorLineEdit](){
            QColor color = QColorDialog::getColor(colorLineEdit.color());
            if(color.isValid()) {
                colorLineEdit.setColor(color);
            }
        });

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

        connect(&buttonBox, &QDialogButtonBox::accepted, [&dial, &messageLineEdit, &watchedParamLineEdit](){
            if(!watchedParamLineEdit.text().isEmpty() &&
                    !messageLineEdit.text().isEmpty() )
                dial.accept();
            else
                QMessageBox::warning(nullptr, "Alarm Creation", "Error : incomplete configuration");

        });
        connect(&buttonBox, SIGNAL(rejected()), &dial, SLOT(reject()));
        dialLayout.addWidget(&buttonBox);


        dial.setLayout(&dialLayout);
        int res = dial.exec();
        if( res == QDialog::Accepted) {
            ui->alarmsTableWidget->setItem(row,
                                           0,
                                           new QTableWidgetItem(activeCheck.isChecked() ? QString("Yes") : QString("No")));

            ui->alarmsTableWidget->setItem(row,
                                           1,
                                           new QTableWidgetItem(watchedParamLineEdit.text()));

            ui->alarmsTableWidget->setItem(row,
                                           2,
                                           new QTableWidgetItem(functionCombo.currentText()));

            ui->alarmsTableWidget->setItem(row,
                                           3,
                                           new QTableWidgetItem(QString::number(argSpinBox.value())));

            ui->alarmsTableWidget->setItem(row,
                                           4,
                                           new QTableWidgetItem(messageLineEdit.text()));

            ui->alarmsTableWidget->setItem(row,
                                           5,
                                           new QTableWidgetItem(displayedParamLineEdit.text()));

            auto colorItem = new QTableWidgetItem(colorLineEdit.color().name());
            colorItem->setForeground(colorLineEdit.color());

            ui->alarmsTableWidget->setItem(row,
                                           6,
                                           colorItem);
            mModified = true;
        }

    }
}

void QTBAlarmConfigurationWidget::resizeEvent(QResizeEvent *event)
{
    resizeColumns();
    QWidget::resizeEvent(event);
}

void QTBAlarmConfigurationWidget::resizeColumns()
{
    ui->alarmsTableWidget->resizeColumnsToContents();
    int width = 0;
    for(int i=0; i< ui->alarmsTableWidget->columnCount(); i++)
        width += ui->alarmsTableWidget->columnWidth(i);

    if(width < ui->alarmsTableWidget->width()) {
        double ratio = double(ui->alarmsTableWidget->width()) / double(width);

        for(int i=0; i< ui->alarmsTableWidget->columnCount(); i++)
            ui->alarmsTableWidget->setColumnWidth(i, ratio * ui->alarmsTableWidget->columnWidth(i) - 1);
    }
}

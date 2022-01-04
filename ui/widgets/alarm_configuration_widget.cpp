#include "alarm_configuration_widget.h"
#include "ui_alarm_configuration_widget.h"
#include "common/color_line_edit.h"
#include "ui/board/management/utils/string_util.h"
#include <QtGlobal>

AlarmConfigurationWidget::AlarmConfigurationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmConfigurationWidget)
{
    ui->setupUi(this);
    ui->alarmsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mModified = false;

    connect(ui->newPushButton,&QPushButton::clicked, this, &AlarmConfigurationWidget::newAlarm );
    connect(ui->downPushButton,&QPushButton::clicked, this, &AlarmConfigurationWidget::moveDown );
    connect(ui->upPushButton,&QPushButton::clicked, this, &AlarmConfigurationWidget::moveUp );
    connect(ui->delPushButton,&QPushButton::clicked, this, &AlarmConfigurationWidget::removeAlarm );
    connect(ui->filterPushButton,&QPushButton::clicked, this, &AlarmConfigurationWidget::filter );
    connect(ui->resetFilterPushButton,&QPushButton::clicked, this, &AlarmConfigurationWidget::clearFilter );
    connect(ui->alarmsTableWidget, &QTableWidget::itemDoubleClicked, this, &AlarmConfigurationWidget::doubleClickedItem);
    connect(ui->connectedCheckBox, &QCheckBox::stateChanged,this, &AlarmConfigurationWidget::stateChanged);
}

AlarmConfigurationWidget::~AlarmConfigurationWidget()
{
    delete ui;
}

void AlarmConfigurationWidget::setEditionMode(const AlarmConfigurationWidget::EditionMode &mode)
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

void AlarmConfigurationWidget::updateConfig()
{
    auto name = ui->configNameLineEdit->text();
    mAlarmConfig->setName(name);
    mAlarmConfig->clearAlarms();

    for(int i=0; i< ui->alarmsTableWidget->rowCount(); i++) {
        bool active = true;
        if(ui->alarmsTableWidget->item(i, 0)->text() == QString("No"))
            active = false;

        Alarms::AlarmFunction func = Alarms::afBitSet;
        if(ui->alarmsTableWidget->item(i, 2)->text() == QString("Greater than X"))
            func = Alarms::afGreaterThan;
        if(ui->alarmsTableWidget->item(i, 2)->text() == QString("Lower than X"))
            func = Alarms::afLowerThan;
        if(ui->alarmsTableWidget->item(i, 2)->text() == QString("Parameter not alive"))
            func = Alarms::afParamDead;

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

void AlarmConfigurationWidget::updateUi()
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
        case Alarms::afBitSet:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Bit X Set"));
            break;
        case Alarms::afGreaterThan:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Greater than X"));
            break;
        case Alarms::afLowerThan:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Lower than X"));
            break;
        case Alarms::afParamDead:

            ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                           2,
                                           new QTableWidgetItem("Parameter not alive"));
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

        auto colorItem = new QTableWidgetItem(mAlarmConfig->colors().at(i).name(QColor::HexArgb));
        colorItem->setForeground(mAlarmConfig->colors().at(i));

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       6,
                                       colorItem);
    }
}

bool AlarmConfigurationWidget::isConnected()
{
    return ui->connectedCheckBox->isChecked();
}

QString AlarmConfigurationWidget::getUiName()
{
    return ui->configNameLineEdit->text();
}

QExplicitlySharedDataPointer<Alarms> AlarmConfigurationWidget::alarmConfig() const
{
    return mAlarmConfig;
}

void AlarmConfigurationWidget::setAlarmConfig(const QExplicitlySharedDataPointer<Alarms> &alarmConfig)
{
    mAlarmConfig = alarmConfig;
}

void AlarmConfigurationWidget::filter()
{
    int nbRow = ui->alarmsTableWidget->rowCount();
    for(int i=0; i< nbRow; i++)
        ui->alarmsTableWidget->setRowHidden(i, false);

    if(!ui->watchedParamLineEdit->text().isEmpty()) {
        QList<int> rowsToShow;
        auto items = ui->alarmsTableWidget->findItems(ui->watchedParamLineEdit->text(), Qt::MatchExactly);
        for(auto item : items)
            if(item->column() == 1)
                rowsToShow.append(item->row());

        for(int i=0; i< nbRow; i++) {
            if(rowsToShow.contains(i)) {
                if(!ui->alarmsTableWidget->isRowHidden(i))
                    ui->alarmsTableWidget->setRowHidden(i, false);
            } else {
                ui->alarmsTableWidget->setRowHidden(i, true);
            }
        }
    }

    if(!ui->messageLineEdit->text().isEmpty()) {
        QList<int> rowsToShow;
        auto items = ui->alarmsTableWidget->findItems(ui->messageLineEdit->text(), Qt::MatchContains);
        for(auto item : items)
            if(item->column() == 4)
                rowsToShow.append(item->row());

        for(int i=0; i< nbRow; i++) {
            if(rowsToShow.contains(i)) {
                if(!ui->alarmsTableWidget->isRowHidden(i))
                    ui->alarmsTableWidget->setRowHidden(i, false);
            } else {
                ui->alarmsTableWidget->setRowHidden(i, true);
            }
        }
    }

    if(!ui->displayedParamLineEdit->text().isEmpty()) {
        QList<int> rowsToShow;
        auto items = ui->alarmsTableWidget->findItems(ui->displayedParamLineEdit->text(), Qt::MatchContains);
        for(auto item : items)
            if(item->column() == 5)
                rowsToShow.append(item->row());

        for(int i=0; i< nbRow; i++) {
            if(rowsToShow.contains(i)) {
                if(!ui->alarmsTableWidget->isRowHidden(i))
                    ui->alarmsTableWidget->setRowHidden(i, false);
            } else {
                ui->alarmsTableWidget->setRowHidden(i, true);
            }
        }
    }
}

void AlarmConfigurationWidget::clearFilter()
{
    ui->displayedParamLineEdit->clear();
    ui->messageLineEdit->clear();
    ui->watchedParamLineEdit->clear();

    filter();
}

void AlarmConfigurationWidget::newAlarm()
{
    QDialog dial;
    dial.setWindowTitle("New watched parameter");
    QVBoxLayout dialLayout;
    QHBoxLayout layout;
    QVBoxLayout textLayout;
    QVBoxLayout widgetsLayout;

    QLabel activeLabel("Enabled");
    QCheckBox activeCheck;
    textLayout.addWidget(&activeLabel);
    widgetsLayout.addWidget(&activeCheck);

    QLabel watchedParamLabel("Watched Parameter");
    QLineEdit watchedParamLineEdit("");
    textLayout.addWidget(&watchedParamLabel);
    widgetsLayout.addWidget(&watchedParamLineEdit);

    QLabel functionLabel("Function");
    QComboBox functionCombo;
    functionCombo.addItem("Greater than X");
    functionCombo.addItem("Lower than X");
    functionCombo.addItem("Bit X Set");
    functionCombo.addItem("Parameter not alive");
    textLayout.addWidget(&functionLabel);
    widgetsLayout.addWidget(&functionCombo);

    QLabel functionArgLabel("Function Arg Value");
    QDoubleSpinBox argSpinBox;
    argSpinBox.setRange(-9999999999999999.0, 999999999999999999999.0);
    argSpinBox.setDecimals(4);
    textLayout.addWidget(&functionArgLabel);
    widgetsLayout.addWidget(&argSpinBox);

    QLabel messageLabel("Displayed Message");
    QLineEdit messageLineEdit("");
    textLayout.addWidget(&messageLabel);
    widgetsLayout.addWidget(&messageLineEdit);

    connect(&functionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&argSpinBox](int index) {
        if(index == 2)
            argSpinBox.setDecimals(0);
        else
            argSpinBox.setDecimals(4);

        if(index == 3)
            argSpinBox.setEnabled(false);
        else
            argSpinBox.setEnabled(true);
    });


    QLabel didsplayedParamLabel("Displayed Parameter Label");
    QLineEdit displayedParamLineEdit("");
    textLayout.addWidget(&didsplayedParamLabel);
    widgetsLayout.addWidget(&displayedParamLineEdit);


    QLabel colorLabel("Text color");
    QHBoxLayout colorLayout;
    QPushButton colorButton;
    colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
    ColorLineEdit colorLineEdit;
    colorLineEdit.setMode(ColorSettings::cmNoBackground);
    colorLayout.addWidget(&colorLineEdit);
    colorLayout.addWidget(&colorButton);

    textLayout.addWidget(&colorLabel);
    widgetsLayout.addLayout(&colorLayout);

    connect(&colorButton, &QPushButton::clicked, [&colorLineEdit](){
        QColor color = QColorDialog::getColor(colorLineEdit.color(), nullptr, QString("Color picker"), QColorDialog::ShowAlphaChannel);
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

    layout.addLayout(&textLayout);
    layout.addLayout(&widgetsLayout);

    dialLayout.addLayout(&layout);
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

        auto colorItem = new QTableWidgetItem(colorLineEdit.color().name(QColor::HexArgb));
        colorItem->setForeground(colorLineEdit.color());

        ui->alarmsTableWidget->setItem(ui->alarmsTableWidget->rowCount() - 1,
                                       6,
                                       colorItem);

        mModified = true;
    }
}

void AlarmConfigurationWidget::moveDown()
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

void AlarmConfigurationWidget::moveUp()
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

void AlarmConfigurationWidget::removeAlarm()
{
    QList<QTableWidgetItem*> items = ui->alarmsTableWidget->selectedItems();
    if(items.count() > 0) {
        int row = items.at(0)->row();
        ui->alarmsTableWidget->removeRow(row);
    }
}

void AlarmConfigurationWidget::doubleClickedItem(QTableWidgetItem *item)
{
    if(item) {
        int row = item->row();


        QDialog dial;
        dial.setWindowTitle("Edit watched parameter");
        QVBoxLayout dialLayout;
        QHBoxLayout layout;
        QVBoxLayout textLayout;
        QVBoxLayout widgetsLayout;

        QLabel activeLabel("Enabled");
        QCheckBox activeCheck;
        textLayout.addWidget(&activeLabel);
        widgetsLayout.addWidget(&activeCheck);

        QLabel watchedParamLabel("Watched Parameter");
        QLineEdit watchedParamLineEdit("");
        textLayout.addWidget(&watchedParamLabel);
        widgetsLayout.addWidget(&watchedParamLineEdit);

        QLabel functionLabel("Function");
        QComboBox functionCombo;
        functionCombo.addItem("Greater than X");
        functionCombo.addItem("Lower than X");
        functionCombo.addItem("Bit X Set");
        functionCombo.addItem("Parameter not alive");
        textLayout.addWidget(&functionLabel);
        widgetsLayout.addWidget(&functionCombo);

        QLabel functionArgLabel("Function Arg Value");
        QDoubleSpinBox argSpinBox;
        argSpinBox.setRange(-9999999999999999.0, 999999999999999999999.0);
        argSpinBox.setDecimals(4);
        textLayout.addWidget(&functionArgLabel);
        widgetsLayout.addWidget(&argSpinBox);

        QLabel messageLabel("Displayed Message");
        QLineEdit messageLineEdit("");
        textLayout.addWidget(&messageLabel);
        widgetsLayout.addWidget(&messageLineEdit);

        connect(&functionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&argSpinBox](int index) {
            if(index == 2)
                argSpinBox.setDecimals(0);
            else
                argSpinBox.setDecimals(4);

            if(index == 3)
                argSpinBox.setEnabled(false);
            else
                argSpinBox.setEnabled(true);
        });


        QLabel didsplayedParamLabel("Displayed Parameter Label");
        QLineEdit displayedParamLineEdit("");
        textLayout.addWidget(&didsplayedParamLabel);
        widgetsLayout.addWidget(&displayedParamLineEdit);


        QLabel colorLabel("Text color");
        QHBoxLayout colorLayout;
        QPushButton colorButton;
        colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
        ColorLineEdit colorLineEdit;
        colorLineEdit.setMode(ColorSettings::cmNoBackground);
        colorLayout.addWidget(&colorLineEdit);
        colorLayout.addWidget(&colorButton);

        textLayout.addWidget(&colorLabel);
        widgetsLayout.addLayout(&colorLayout);

        connect(&colorButton, &QPushButton::clicked, [&colorLineEdit](){
            QColor color = QColorDialog::getColor(colorLineEdit.color(), nullptr, QString("Color picker"), QColorDialog::ShowAlphaChannel);
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

        layout.addLayout(&textLayout);
        layout.addLayout(&widgetsLayout);

        dialLayout.addLayout(&layout);
        dialLayout.addWidget(&buttonBox);

        dial.setLayout(&dialLayout);

        if(ui->alarmsTableWidget->item(row, 0)->text() == QString("Yes"))
            activeCheck.setChecked(true);
        else
            activeCheck.setChecked(false);
        watchedParamLineEdit.setText(ui->alarmsTableWidget->item(row, 1)->text());
        functionCombo.setCurrentText(ui->alarmsTableWidget->item(row, 2)->text());
        argSpinBox.setValue(ui->alarmsTableWidget->item(row, 3)->text().toDouble());
        messageLineEdit.setText(ui->alarmsTableWidget->item(row, 4)->text());


        displayedParamLineEdit.setText(ui->alarmsTableWidget->item(row, 5)->text());

        colorLineEdit.setColor(QColor(ui->alarmsTableWidget->item(row, 6)->text()));

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

            auto colorItem = new QTableWidgetItem(colorLineEdit.color().name(QColor::HexArgb));
            colorItem->setForeground(colorLineEdit.color());

            ui->alarmsTableWidget->setItem(row,
                                           6,
                                           colorItem);
            mModified = true;
        }

    }
}

void AlarmConfigurationWidget::resizeEvent(QResizeEvent *event)
{
//    resizeColumns();
    QWidget::resizeEvent(event);
}

void AlarmConfigurationWidget::resizeColumns()
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

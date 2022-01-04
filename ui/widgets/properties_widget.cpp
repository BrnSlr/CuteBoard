#include "properties_widget.h"
#include "ui_properties_widget.h"
#include <QColorDialog>
#include <QMessageBox>
#include "ui/widgets/utils/icon_factory.h"

PropertiesWidget::PropertiesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertiesWidget),
    mProject(nullptr),
    mStatesModified(false),
    mThresholdsModified(false),
    mBitfieldsModified(false)
{
    ui->setupUi(this);
    ui->propertiesStackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    QIcon actionAdd_icon = create_icon(":/icons8_plus_math_filled_50px.png");
    QAction *actionAddState = new QAction(actionAdd_icon, QString(""));
    ui->addStateToolButton->setDefaultAction(actionAddState);

    QAction *actionAddHighThreshold = new QAction(actionAdd_icon, QString(""));
    ui->addHighThresholdToolButton->setDefaultAction(actionAddHighThreshold);

    QAction *actionAddLowThreshold = new QAction(actionAdd_icon, QString(""));
    ui->addLowThresholdToolButton->setDefaultAction(actionAddLowThreshold);

    connect(ui->createConfigurationPushButton, &QPushButton::clicked, this, &PropertiesWidget::createSharedConfiguration);
}

PropertiesWidget::~PropertiesWidget()
{
    delete ui;
}

bool PropertiesWidget::isConnected()
{
    return ui->connectedCheckBox->isChecked();
}

void PropertiesWidget::updateUi(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    if(!parameterSettings) {
        logger()->debug() << "param null";
        return;
    }
    mCurrentSettings = parameterSettings;

    ui->descriptionLineEdit->setText(parameterSettings->description());
    ui->labelLineEdit->setText(parameterSettings->label());
    ui->userLabelLineEdit->setText(parameterSettings->userDefinedLabel());
    ui->userLabelCheckBox->setChecked(parameterSettings->userLabelEnabled());
    ui->userUnitLineEdit->setText(parameterSettings->userDefinedUnit());
    ui->userUnitCheckBox->setChecked(parameterSettings->userUnitEnabled());
    ui->precisionSpinBox->setValue(parameterSettings->precision());
    ui->validRangeCheckBox->setChecked(parameterSettings->validRange());
    ui->rangeMaxDoubleSpinBox->setValue(parameterSettings->rangeMaximum());
    ui->rangeMinDoubleSpinBox->setValue(parameterSettings->rangeMinimum());
    ui->colorLineEdit->setColor(parameterSettings->defaultColorSettingsRef().color());
    ui->colorLineEdit->setMode(parameterSettings->defaultColorSettingsRef().mode());
    ui->colorModeComboBox->setCurrentIndex(parameterSettings->defaultColorSettingsRef().mode()-1);

    ui->itemsColorComboBox->setCurrentIndex(parameterSettings->itemColorMode());
    if(parameterSettings->itemColorMode() == ParameterConfiguration::icCustom ) {
        ui->itemsColorWidget->setVisible(true);
        ui->itemsColorLineEdit->setText(parameterSettings->itemStaticColor().name(QColor::HexArgb));
    } else {
        ui->itemsColorWidget->setVisible(false);
        ui->itemsColorLineEdit->setText(parameterSettings->itemStaticColor().name(QColor::HexArgb));
    }

    ui->outOfRangeCheckBox->setChecked(parameterSettings->outOfRangeColorEnabled());
    ui->outOfRangeColorLineEdit->setColor(parameterSettings->outOfRangeColorSettingsRef().color());
    ui->outOfRangeColorLineEdit->setMode(parameterSettings->outOfRangeColorSettingsRef().mode());
    ui->outOfRangeColorModeComboBox->setCurrentIndex(parameterSettings->outOfRangeColorSettingsRef().mode()-1);

    ui->lowThresholdsTableWidget->clear();
    QMapIterator<double, ColorSettings> itLow(parameterSettings->thresholdsSettingsRef().lowThresholds());
    while (itLow.hasNext()) {
        itLow.next();

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(itLow.value().mode());
        newLine->setColor(itLow.value().color());
        newLine->setAlignment(Qt::AlignCenter);
        newLine->setReadOnly(true);
        newLine->setText(QString::number(itLow.key()));
        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

        ui->lowThresholdsTableWidget->insertRow(0);
        ui->lowThresholdsTableWidget->setCellWidget(0,0,btn_edit);
        ui->lowThresholdsTableWidget->setCellWidget(0,1,newLine);
    }
    ui->lowThresholdsTableWidget->resizeColumnToContents(0);

    ui->highThresholdsTableWidget->clear();
    QMapIterator<double, ColorSettings> itHigh(parameterSettings->thresholdsSettingsRef().highThresholds());
    while (itHigh.hasNext()) {
        itHigh.next();

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(itHigh.value().mode());
        newLine->setColor(itHigh.value().color());
        newLine->setAlignment(Qt::AlignCenter);
        newLine->setReadOnly(true);
        newLine->setText(QString::number(itHigh.key()));
        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);


        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

        ui->highThresholdsTableWidget->insertRow(0);
        ui->highThresholdsTableWidget->setCellWidget(0,0,btn_edit);
        ui->highThresholdsTableWidget->setCellWidget(0,1,newLine);
    }
    ui->highThresholdsTableWidget->resizeColumnToContents(0);

    ui->statesTableWidget->clear();
    QMapIterator<qlonglong, ColorSettings> itStateColor(parameterSettings->statesSettingsRef().statesColor());
    while (itStateColor.hasNext()) {
        itStateColor.next();

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(itStateColor.key()));
        item->setTextAlignment(Qt::AlignCenter);

        ColorLineEdit *newLine2 = new ColorLineEdit();
        newLine2->setMode(itStateColor.value().mode());
        newLine2->setColor(itStateColor.value().color());
        newLine2->setAlignment(Qt::AlignLeft);
        newLine2->setReadOnly(true);
        newLine2->setText(parameterSettings->statesSettingsRef().statesText().value(itStateColor.key()));

        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeState_clicked);

        ui->statesTableWidget->insertRow(0);
        ui->statesTableWidget->setCellWidget(0,0,btn_edit);
        ui->statesTableWidget->setItem(0,1,item);
        ui->statesTableWidget->setCellWidget(0,2,newLine2);
    }
    ui->statesTableWidget->resizeColumnToContents(0);

    for(int i=0; i<32;i++) {
        QCheckBox *check = new QCheckBox();
        check->setChecked(parameterSettings->bitfieldsSettingsRef().bitLogics().at(i));
        connect(check, &QCheckBox::stateChanged, [=](){
            mBitfieldsModified = true;
        });

        ui->bitfieldsTableWidget->setCellWidget(31-i,0,check);
        QTableWidgetItem *item = new QTableWidgetItem(parameterSettings->bitfieldsSettingsRef().bitDescriptions().at(i));
        ui->bitfieldsTableWidget->setItem(31-i,1,item);

    }
    connect(ui->bitfieldsTableWidget, &QTableWidget::itemChanged, [=](){
        mBitfieldsModified = true;
    });

    ui->scatterStyleComboBox->setCurrentIndex(parameterSettings->scatterShape());
    ui->scatterSizeSpinBox->setValue(parameterSettings->scatterSize());
    ui->lineWidthSpinBox->setValue(parameterSettings->penWidth());

    ui->brushStyleComboBox->setCurrentIndex(parameterSettings->graphBrush());
    if(parameterSettings->graphLineStyle() == QCPGraph::lsImpulse)
        ui->graphLineStyleComboBox->setCurrentIndex(2);
    else
        ui->graphLineStyleComboBox->setCurrentIndex(parameterSettings->graphLineStyle());
    ui->itemsThresholdsCheckBox->setChecked(parameterSettings->itemsThresholdsVisible());

    ui->curveTracerCheckBox->setChecked(parameterSettings->curveTracerVisible());
}

void PropertiesWidget::updateParameterSettings(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    parameterSettings->setDescription(ui->descriptionLineEdit->text());

    parameterSettings->setLabel(ui->labelLineEdit->text());
    parameterSettings->setUserDefinedLabel(ui->userLabelLineEdit->text());
    parameterSettings->setUserLabelEnabled(ui->userLabelCheckBox->isChecked());

    parameterSettings->setUserDefinedUnit(ui->userUnitLineEdit->text());
    parameterSettings->setUserUnitEnabled(ui->userUnitCheckBox->isChecked());

    parameterSettings->setPrecision(ui->precisionSpinBox->value());

    parameterSettings->setValidRange(ui->validRangeCheckBox->isChecked());
    parameterSettings->setRangeMaximum(ui->rangeMaxDoubleSpinBox->value());
    parameterSettings->setRangeMinimum(ui->rangeMinDoubleSpinBox->value());

    parameterSettings->defaultColorSettingsRef().setColor(ui->colorLineEdit->color());
    parameterSettings->defaultColorSettingsRef().setMode(ui->colorLineEdit->mode());

    parameterSettings->setOutOfRangeColorEnabled(ui->outOfRangeCheckBox->isChecked());
    parameterSettings->outOfRangeColorSettingsRef().setColor(ui->outOfRangeColorLineEdit->color());
    parameterSettings->outOfRangeColorSettingsRef().setMode(ui->outOfRangeColorLineEdit->mode());

    if(mThresholdsModified) {
        parameterSettings->thresholdsSettingsRef().clearHighThresholds();
        for(int i=0; i<ui->highThresholdsTableWidget->rowCount();i++) {
            ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->highThresholdsTableWidget->cellWidget(i, 1));
            if(w) {
                double val = w->text().toDouble();
                ColorSettings cs;
                cs.setMode(w->mode());
                cs.setColor(w->color());
                parameterSettings->thresholdsSettingsRef().insertHighThreshold(val, cs);
            }
        }

        parameterSettings->thresholdsSettingsRef().clearLowThresholds();
        for(int i=0; i<ui->lowThresholdsTableWidget->rowCount();i++) {
            ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->lowThresholdsTableWidget->cellWidget(i, 1));
            if(w) {
                double val = w->text().toDouble();
                ColorSettings cs;
                cs.setMode(w->mode());
                cs.setColor(w->color());
                parameterSettings->thresholdsSettingsRef().insertLowThreshold(val, cs);
            }
        }
    }

    if(mStatesModified) {
        parameterSettings->statesSettingsRef().clearStates();
        for(int i=0; i<ui->statesTableWidget->rowCount();i++) {
            ColorLineEdit *cl = qobject_cast<ColorLineEdit *>(ui->statesTableWidget->cellWidget(i, 2));
            if(cl) {
                qlonglong val = ui->statesTableWidget->item(i,1)->text().toLongLong();

                ColorSettings cs;
                cs.setMode(cl->mode());
                cs.setColor(cl->color());

                parameterSettings->statesSettingsRef().addState(true, val, cl->text(), cs);
            }
        }
    }

    if(mBitfieldsModified) {
        for(int i=0; i<32;i++) {
            QCheckBox *check = qobject_cast<QCheckBox *>(ui->bitfieldsTableWidget->cellWidget(31-i,0));
            if(check) {
                parameterSettings->bitfieldsSettingsRef().setBitLogic(i, check->isChecked());
            }
            parameterSettings->bitfieldsSettingsRef().setBitDescription(i, ui->bitfieldsTableWidget->item(31-i,1)->text());
        }
    }

    parameterSettings->setScatterShape(QCPScatterStyle::ScatterShape(ui->scatterStyleComboBox->currentIndex()));
    parameterSettings->setScatterSize(ui->scatterSizeSpinBox->value());
    parameterSettings->setPenWidth(ui->lineWidthSpinBox->value());

    parameterSettings->setGraphBrush(ParameterConfiguration::BrushStyle(ui->brushStyleComboBox->currentIndex()));
    parameterSettings->setGraphLineStyle(QCPGraph::LineStyle(ui->graphLineStyleComboBox->currentIndex()));
    parameterSettings->setItemsThresholdsVisible(ui->itemsThresholdsCheckBox->isChecked());

    parameterSettings->setCurveTracerVisible(ui->curveTracerCheckBox->isChecked());

    parameterSettings->setItemColorMode(ParameterConfiguration::ItemColor(ui->itemsColorComboBox->currentIndex()));
    parameterSettings->setItemStaticColor(ui->itemsColorLineEdit->text());
}

void PropertiesWidget::on_colorModeComboBox_currentIndexChanged(int index)
{
    ui->colorLineEdit->setMode(ColorSettings::ColorMode(index+1));
}

void PropertiesWidget::on_defaultColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->colorLineEdit->color(), this, QString("Color picker"), QColorDialog::ShowAlphaChannel);
    if(color.isValid()) {
        ui->colorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_validRangeCheckBox_stateChanged(int arg1)
{
    ui->rangeScaleWidget->setEnabled(arg1);
    ui->rangeMaxDoubleSpinBox->setEnabled(arg1);
    ui->rangeMinDoubleSpinBox->setEnabled(arg1);
    ui->outOfRangeWidget->setEnabled(arg1);
    ui->outOfRangeCheckBox->setEnabled(arg1);
    on_outOfRangeCheckBox_stateChanged(ui->outOfRangeCheckBox->checkState());
}

void PropertiesWidget::on_defaultColorToolButton_2_clicked()
{
    QColor color = QColorDialog::getColor(ui->outOfRangeColorLineEdit->color(), this, QString("Color picker"), QColorDialog::ShowAlphaChannel);
    if(color.isValid()) {
        ui->outOfRangeColorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_outOfRangeColorModeComboBox_currentIndexChanged(int index)
{
    ui->outOfRangeColorLineEdit->setMode(ColorSettings::ColorMode(index+1));
}

void PropertiesWidget::on_listWidget_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow)
    QStringList myOptions;
    myOptions << "General" << "Style" << "Thresholds" << "States" << "Bitfields";
    if(ui->listWidget->currentItem()) {
        if(myOptions.contains(ui->listWidget->currentItem()->text()))
            ui->propertiesStackedWidget->setCurrentIndex(myOptions.indexOf(ui->listWidget->currentItem()->text()));
    }
}

void PropertiesWidget::on_connectedCheckBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) {
        emit connectProperties(true);
    } else {
        emit connectProperties(false);
    }
}

void PropertiesWidget::setEditionMode(const EditionMode &mode)
{
    const QSignalBlocker blocker(this);
    mEditionMode = mode;

    switch(mEditionMode) {
    case emCreation:
        ui->connectedCheckBox->setChecked(true);
        ui->createConfigurationPushButton->setVisible(false);
        ui->configurationWidget->setVisible(true);
        ui->configurationWidget->setEnabled(true);
        ui->connectedCheckBox->setEnabled(false);
        ui->editPropPushButton->setVisible(false);
        ui->editPropPushButton->setEnabled(false);
        ui->connectedWidget->setEnabled(true);
        ui->descriptionWidget->setVisible(true);
        ui->descriptionWidget->setEnabled(true);
        ui->descriptionLineEdit->setEnabled(true);
        ui->labelWidget->setEnabled(true);
        ui->labelLineEdit->setEnabled(true);
        ui->userWidget->setEnabled(true);
        break;
    case emEdition:
        ui->connectedCheckBox->setChecked(true);
        ui->createConfigurationPushButton->setVisible(false);
        ui->configurationWidget->setVisible(true);
        ui->configurationWidget->setEnabled(false);
        ui->editPropPushButton->setVisible(false);
        ui->editPropPushButton->setEnabled(false);
        ui->descriptionWidget->setVisible(true);
        ui->descriptionWidget->setEnabled(false);
        ui->descriptionLineEdit->setEnabled(false);
        ui->labelWidget->setEnabled(false);
        ui->labelLineEdit->setEnabled(false);
        ui->userWidget->setEnabled(true);
        break;
    case emElementConnected:
        ui->connectedCheckBox->setChecked(true);
        ui->createConfigurationPushButton->setVisible(false);
        ui->configurationWidget->setVisible(true);
        ui->configurationWidget->setEnabled(true);
        ui->editPropPushButton->setVisible(true);
        ui->editPropPushButton->setEnabled(true);
        ui->descriptionWidget->setVisible(true);
        ui->descriptionWidget->setEnabled(false);
        ui->descriptionLineEdit->setEnabled(false);
        ui->labelWidget->setEnabled(false);
        ui->labelLineEdit->setEnabled(false);
        ui->userWidget->setEnabled(false);
        for(int i=1; i< ui->propertiesStackedWidget->count();i++)
            ui->propertiesStackedWidget->widget(i)->setEnabled(false);
        break;
    case emElementDisconnected:
        ui->connectedCheckBox->setChecked(false);
        ui->createConfigurationPushButton->setVisible(true);
        ui->configurationWidget->setVisible(true);
        ui->configurationWidget->setEnabled(true);
        ui->editPropPushButton->setVisible(true);
        ui->editPropPushButton->setEnabled(false);
        ui->descriptionWidget->setVisible(true);
        ui->descriptionWidget->setEnabled(false);
        ui->descriptionLineEdit->setEnabled(false);
        ui->labelWidget->setEnabled(false);
        ui->labelLineEdit->setEnabled(false);
        ui->userWidget->setEnabled(true);
        for(int i=1; i< ui->propertiesStackedWidget->count();i++)
            ui->propertiesStackedWidget->widget(i)->setEnabled(true);
        break;
    case emElementStandAlone:
        ui->connectedCheckBox->setChecked(false);
        ui->createConfigurationPushButton->setVisible(true);
        ui->configurationWidget->setVisible(false);
        ui->configurationWidget->setEnabled(false);
        ui->editPropPushButton->setVisible(false);
        ui->editPropPushButton->setEnabled(false);
        ui->descriptionWidget->setVisible(false);
        ui->descriptionWidget->setEnabled(false);
        ui->descriptionLineEdit->setEnabled(false);
        ui->labelWidget->setEnabled(false);
        ui->labelLineEdit->setEnabled(false);
        ui->userWidget->setEnabled(true);
        break;
    }
}

void PropertiesWidget::on_outOfRangeCheckBox_stateChanged(int arg1)
{
    ui->outOfRangeColorWidget->setEnabled(arg1);
    ui->defaultColorToolButton_2->setEnabled(arg1);
}

void PropertiesWidget::removeHighThreshold_clicked()
{
    for(int i = 0; i < ui->highThresholdsTableWidget->rowCount(); i++)
    {
        if(ui->highThresholdsTableWidget->cellWidget(i, 0) == QObject::sender())
        {
            ui->highThresholdsTableWidget->removeRow(i);
            mThresholdsModified = true;
            return;
        }
    }
}

void PropertiesWidget::removeLowThreshold_clicked()
{
    for(int i = 0; i < ui->lowThresholdsTableWidget->rowCount(); i++)
    {
        if(ui->lowThresholdsTableWidget->cellWidget(i, 0) == QObject::sender())
        {
            ui->lowThresholdsTableWidget->removeRow(i);
            mThresholdsModified = true;
            return;
        }
    }
}

void PropertiesWidget::removeState_clicked()
{
    mStatesModified = true;
    for(int i = 0; i < ui->statesTableWidget->rowCount(); i++)
    {
        if(ui->statesTableWidget->cellWidget(i, 0) == QObject::sender())
        {
            ui->statesTableWidget->removeRow(i);
            return;
        }
    }
}

void PropertiesWidget::setPropertiesMode(const ParameterConfiguration::ConfigurationMode &propertiesMode)
{
    mPropertiesMode = propertiesMode;

    ui->listWidget->clear();
    ui->listWidget->addItem("General");
    switch(mPropertiesMode) {
    case ParameterConfiguration::cmFull:
        ui->listWidget->addItem("Style");
        ui->listWidget->addItem("Thresholds");
        ui->listWidget->addItem("States");
        ui->listWidget->addItem("Bitfields");
        ui->itemsStyleWidget->setVisible(true);
        ui->brushStyleComboBox->setEnabled(true);
        break;
    case ParameterConfiguration::cmValue:
        ui->listWidget->addItem("Thresholds");
        ui->itemsStyleWidget->setVisible(false);
        break;
    case ParameterConfiguration::cmState:
        ui->listWidget->addItem("States");
        ui->itemsStyleWidget->setVisible(false);
        break;
    case ParameterConfiguration::cmCurveX:
        ui->listWidget->addItem("Style");
        ui->listWidget->addItem("Thresholds");
        ui->itemsStyleWidget->setVisible(false);
        ui->brushStyleComboBox->setEnabled(false);
        ui->curveWidget->setVisible(false);
        ui->graphWidget->setVisible(false);
        break;
    case ParameterConfiguration::cmCurveY:
        ui->listWidget->addItem("Style");
        ui->listWidget->addItem("Thresholds");
        ui->itemsStyleWidget->setVisible(true);
        ui->brushStyleComboBox->setEnabled(false);
        ui->curveWidget->setVisible(true);
        ui->graphWidget->setVisible(false);
        break;
    case ParameterConfiguration::cmGraph:
        ui->listWidget->addItem("Style");
        ui->listWidget->addItem("Thresholds");
        ui->itemsStyleWidget->setVisible(true);
        ui->brushStyleComboBox->setEnabled(true);
        ui->curveWidget->setVisible(false);
        ui->graphWidget->setVisible(true);
        break;
    case ParameterConfiguration::cmBitFields:
        ui->listWidget->addItem("Bitfields");
        ui->itemsStyleWidget->setVisible(false);
        break;
    }
}

void PropertiesWidget::on_itemsColorComboBox_currentIndexChanged(int index)
{
    if(index == 3)
        ui->itemsColorWidget->setVisible(true);
    else
        ui->itemsColorWidget->setVisible(false);
}

void PropertiesWidget::on_itemsColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->itemsColorLineEdit->color(), this, QString("Color picker"), QColorDialog::ShowAlphaChannel);
    if(color.isValid()) {
        ui->itemsColorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_statesTableWidget_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    ColorLineEdit *lineEdit = qobject_cast<ColorLineEdit *>(ui->statesTableWidget->cellWidget(row, 2));
    QTableWidgetItem *itemText = ui->statesTableWidget->item(row, 1);
    if(lineEdit && itemText) {

        QDialog dial;
        dial.setWindowTitle("Edit State");
        QVBoxLayout dialLayout;
        QHBoxLayout layout;
        QVBoxLayout textLayout;
        QVBoxLayout widgetsLayout;

        QLabel valueLabel("Value");
        QSpinBox valueSpinBox;
        valueSpinBox.setRange(-999999999, 999999999);
        textLayout.addWidget(&valueLabel);
        widgetsLayout.addWidget(&valueSpinBox);

        QLabel messageLabel("Message");
        QLineEdit messageLineEdit;
        textLayout.addWidget(&messageLabel);
        widgetsLayout.addWidget(&messageLineEdit);

        QLabel colorLabel("Color");
        QHBoxLayout colorLayout;
        QPushButton colorButton;
        colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
        ColorLineEdit colorLineEdit;
        colorLineEdit.setMode(ColorSettings::cmFilledBackground);
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

        QLabel modeLabel("Mode");
        QComboBox modeCombobox;
        modeCombobox.addItem("Filled Background");
        modeCombobox.addItem("No Background");
        modeCombobox.setCurrentIndex(0);

        connect(&modeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&colorLineEdit](int index) {
            colorLineEdit.setMode(ColorSettings::ColorMode(index+1));
        });

        textLayout.addWidget(&modeLabel);
        widgetsLayout.addWidget(&modeCombobox);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

        connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

        layout.addLayout(&textLayout);
        layout.addLayout(&widgetsLayout);

        dialLayout.addLayout(&layout);
        dialLayout.addWidget(&buttonBox);

        dial.setLayout(&dialLayout);

        auto text = lineEdit->text();
        auto mode = lineEdit->mode();
        auto color = lineEdit->color();
        auto value = itemText->text();

        valueSpinBox.setValue(value.toInt());
        colorLineEdit.setMode(mode);
        colorLineEdit.setColor(color);
        messageLineEdit.setText(text);
        modeCombobox.setCurrentIndex(mode-1);

        int res = dial.exec();
        if( res == QDialog::Accepted) {

            mStatesModified = true;
            int rowCount = 0;
            for(rowCount = 0; rowCount < ui->statesTableWidget->rowCount(); rowCount++) {
                ColorLineEdit *cl = qobject_cast<ColorLineEdit *>(ui->statesTableWidget->cellWidget(rowCount, 2));
                if(cl) {
                    qlonglong val = ui->statesTableWidget->item(rowCount,1)->text().toLongLong();
                    if(valueSpinBox.value() > val) {
                        QTableWidgetItem *item = new QTableWidgetItem(QString::number(valueSpinBox.value()));
                        item->setTextAlignment(Qt::AlignCenter);

                        ColorLineEdit *newLine = new ColorLineEdit();
                        newLine->setMode(colorLineEdit.mode());
                        newLine->setColor(colorLineEdit.color());
                        newLine->setAlignment(Qt::AlignLeft);
                        newLine->setReadOnly(true);
                        newLine->setText(messageLineEdit.text());
                        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

                        QToolButton* btn_edit = new QToolButton();
                        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeState_clicked);

                        ui->statesTableWidget->insertRow(rowCount);
                        ui->statesTableWidget->setCellWidget(rowCount,0,btn_edit);
                        ui->statesTableWidget->setItem(rowCount,1,item);
                        ui->statesTableWidget->setCellWidget(rowCount,2,newLine);
                        ui->statesTableWidget->resizeColumnToContents(0);
                        ui->statesTableWidget->removeRow(itemText->row());
                        return;
                    }
                    if(valueSpinBox.value() == val) {
                        ui->statesTableWidget->item(rowCount,1)->setText(QString::number(valueSpinBox.value()));

                        cl->setMode(colorLineEdit.mode());
                        cl->setColor(colorLineEdit.color());
                        cl->setText(messageLineEdit.text());

                        return;
                    }
                }
            }

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(valueSpinBox.value()));
            item->setTextAlignment(Qt::AlignCenter);

            ColorLineEdit *newLine = new ColorLineEdit();
            newLine->setMode(colorLineEdit.mode());
            newLine->setColor(colorLineEdit.color());
            newLine->setAlignment(Qt::AlignLeft);
            newLine->setReadOnly(true);
            newLine->setText(messageLineEdit.text());
            newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

            QToolButton* btn_edit = new QToolButton();
            btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
            connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeState_clicked);

            ui->statesTableWidget->insertRow(rowCount);
            ui->statesTableWidget->setCellWidget(rowCount,0,btn_edit);
            ui->statesTableWidget->setItem(rowCount,1,item);
            ui->statesTableWidget->setCellWidget(rowCount,2,newLine);
            ui->statesTableWidget->resizeColumnToContents(0);

            ui->statesTableWidget->removeRow(itemText->row());
        }
    }
}

void PropertiesWidget::on_editPropPushButton_clicked()
{
    auto label = ui->labelLineEdit->text();
    auto descr = ui->descriptionLineEdit->text();
    if(mProject) {
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
                parameterSettings->saveToFile(mProject->paramSettingsPath());
                updateUi(parameterSettings);
            }
        }
    }
}

void PropertiesWidget::setProject(QSharedPointer<Project> project)
{
    mProject = project;
}

void PropertiesWidget::createSharedConfiguration()
{
    if(mCurrentSettings) {
        if(!mCurrentSettings->label().isEmpty()) {
            auto configurations = mProject->parametersSettings();
            if(configurations.contains(mCurrentSettings->label())) {
                QStringList items;

                auto descriptions = configurations.value(mCurrentSettings->label());
                for(auto descr : descriptions.keys())
                    items.append(descr);

                bool ok;
                QString itemText = QInputDialog::getItem(nullptr, tr("Create/Edit Configurations:"),
                                                     QString("If you want to update an existing configuration\nfor parameter %1\nselect a configuration and click OK\notherwise a new configuration will be created").arg(mCurrentSettings->label()), items, 0, false, &ok);


                if (ok && !itemText.isEmpty()) {

                    auto descr = itemText;
                    auto target = mProject->parameterSettings(mCurrentSettings->label(), descr);
                    if(target) {
                        QTemporaryFile tmpFile;
                        QSettings settings(tmpFile.fileName(), QSettings::IniFormat);
                        mCurrentSettings->save(&settings,ParameterConfiguration::cmFull);
                        target->load(&settings);
                        target->setDescription(descr);

                        target->saveToFile(mProject->paramSettingsPath());
                        target->setModified(true);
                        setEditionMode(emElementConnected);
                        updateUi(target);
                    }
                } else {

                    QDialog dial;
                    dial.setWindowTitle("Copy Parameter Configuration");
                    QVBoxLayout layout;
                    QHBoxLayout dialLayout;

                    QVBoxLayout layoutLeft;
                    QVBoxLayout layoutRight;

                    QLabel labelLabel("New label");
                    QLineEdit labelLineEdit(&dial);
                    labelLineEdit.setText(mCurrentSettings->label());
                    labelLineEdit.setEnabled(false);

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
                        mCurrentSettings->setDescription(descrLineEdit.text());
                        mCurrentSettings->saveToFile(mProject->paramSettingsPath());
                        mProject->addParameterSettings(mCurrentSettings);
                        setEditionMode(emElementConnected);
                        updateUi(mCurrentSettings);
                    }
                }
            } else {
                QDialog dial;
                dial.setWindowTitle("Copy Parameter Configuration");
                QVBoxLayout layout;
                QHBoxLayout dialLayout;

                QVBoxLayout layoutLeft;
                QVBoxLayout layoutRight;

                QLabel labelLabel("New label");
                QLineEdit labelLineEdit(&dial);
                labelLineEdit.setText(mCurrentSettings->label());
                labelLineEdit.setEnabled(false);

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
                    mCurrentSettings->setDescription(descrLineEdit.text());
                    mCurrentSettings->saveToFile(mProject->paramSettingsPath());
                    mProject->addParameterSettings(mCurrentSettings);
                    setEditionMode(emElementConnected);
                    updateUi(mCurrentSettings);
                }
            }
        }
    }
}

void PropertiesWidget::on_addStateToolButton_clicked()
{
    QDialog dial;
    dial.setWindowTitle("Add state");
    QVBoxLayout dialLayout;
    QHBoxLayout layout;
    QVBoxLayout textLayout;
    QVBoxLayout widgetsLayout;

    QLabel valueLabel("Value");
    QSpinBox valueSpinBox;
    valueSpinBox.setRange(-999999999, 999999999);
    textLayout.addWidget(&valueLabel);
    widgetsLayout.addWidget(&valueSpinBox);

    QLabel messageLabel("Message");
    QLineEdit messageLineEdit;
    textLayout.addWidget(&messageLabel);
    widgetsLayout.addWidget(&messageLineEdit);

    QLabel colorLabel("Color");
    QHBoxLayout colorLayout;
    QPushButton colorButton;
    colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
    ColorLineEdit colorLineEdit;
    colorLineEdit.setMode(ColorSettings::cmFilledBackground);
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

    QLabel modeLabel("Mode");
    QComboBox modeCombobox;
    modeCombobox.addItem("Filled Background");
    modeCombobox.addItem("No Background");
    modeCombobox.setCurrentIndex(0);

    connect(&modeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&colorLineEdit](int index) {
        colorLineEdit.setMode(ColorSettings::ColorMode(index+1));
    });

    textLayout.addWidget(&modeLabel);
    widgetsLayout.addWidget(&modeCombobox);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

    layout.addLayout(&textLayout);
    layout.addLayout(&widgetsLayout);

    dialLayout.addLayout(&layout);
    dialLayout.addWidget(&buttonBox);

    dial.setLayout(&dialLayout);

    int res = dial.exec();
    if( res == QDialog::Accepted) {
        mStatesModified = true;
        int rowCount = 0;
        for(rowCount = 0; rowCount < ui->statesTableWidget->rowCount(); rowCount++) {
            ColorLineEdit *cl = qobject_cast<ColorLineEdit *>(ui->statesTableWidget->cellWidget(rowCount, 2));
            if(cl) {
                qlonglong val = ui->statesTableWidget->item(rowCount,1)->text().toLongLong();
                if(valueSpinBox.value() > val) {
                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(valueSpinBox.value()));
                    item->setTextAlignment(Qt::AlignCenter);

                    ColorLineEdit *newLine = new ColorLineEdit();
                    newLine->setMode(colorLineEdit.mode());
                    newLine->setColor(colorLineEdit.color());
                    newLine->setAlignment(Qt::AlignLeft);
                    newLine->setReadOnly(true);
                    newLine->setText(messageLineEdit.text());
                    newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

                    QToolButton* btn_edit = new QToolButton();
                    btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                    connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeState_clicked);

                    ui->statesTableWidget->insertRow(rowCount);
                    ui->statesTableWidget->setCellWidget(rowCount,0,btn_edit);
                    ui->statesTableWidget->setItem(rowCount,1,item);
                    ui->statesTableWidget->setCellWidget(rowCount,2,newLine);
                    ui->statesTableWidget->resizeColumnToContents(0);
                    return;
                }
                if(valueSpinBox.value() == val) {
                    ui->statesTableWidget->item(rowCount,1)->setText(QString::number(valueSpinBox.value()));

                    cl->setMode(colorLineEdit.mode());
                    cl->setColor(colorLineEdit.color());
                    cl->setText(messageLineEdit.text());

                    return;
                }
            }
        }

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(valueSpinBox.value()));
        item->setTextAlignment(Qt::AlignCenter);

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(colorLineEdit.mode());
        newLine->setColor(colorLineEdit.color());
        newLine->setAlignment(Qt::AlignLeft);
        newLine->setReadOnly(true);
        newLine->setText(messageLineEdit.text());
        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeState_clicked);

        ui->statesTableWidget->insertRow(rowCount);
        ui->statesTableWidget->setCellWidget(rowCount,0,btn_edit);
        ui->statesTableWidget->setItem(rowCount,1,item);
        ui->statesTableWidget->setCellWidget(rowCount,2,newLine);
        ui->statesTableWidget->resizeColumnToContents(0);
    }
}

void PropertiesWidget::on_addHighThresholdToolButton_clicked()
{
    QDialog dial;
    dial.setWindowTitle(QString("Add high threshold"));
    QVBoxLayout dialLayout;
    QHBoxLayout layout;
    QVBoxLayout textLayout;
    QVBoxLayout widgetsLayout;

    QLabel valueLabel("Value");
    QDoubleSpinBox valueSpinBox;
    valueSpinBox.setLocale(QLocale("C"));
    valueSpinBox.setRange(-999999999., 999999999.);
    textLayout.addWidget(&valueLabel);
    widgetsLayout.addWidget(&valueSpinBox);


    QLabel colorLabel("Color");
    QHBoxLayout colorLayout;
    QPushButton colorButton;
    colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
    ColorLineEdit colorLineEdit;
    colorLineEdit.setMode(ColorSettings::cmFilledBackground);
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

    QLabel modeLabel("Mode");
    QComboBox modeCombobox;
    modeCombobox.addItem("Filled Background");
    modeCombobox.addItem("No Background");
    modeCombobox.setCurrentIndex(0);

    connect(&modeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&colorLineEdit](int index) {
        colorLineEdit.setMode(ColorSettings::ColorMode(index+1));
    });


    textLayout.addWidget(&modeLabel);
    widgetsLayout.addWidget(&modeCombobox);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

    layout.addLayout(&textLayout);
    layout.addLayout(&widgetsLayout);

    dialLayout.addLayout(&layout);
    dialLayout.addWidget(&buttonBox);

    dial.setLayout(&dialLayout);

    int res = dial.exec();
    if( res == QDialog::Accepted) {
        mThresholdsModified = true;
        int rowCount = 0;
        for(rowCount = 0; rowCount < ui->highThresholdsTableWidget->rowCount(); rowCount++) {
            ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->highThresholdsTableWidget->cellWidget(rowCount, 1));
            if(w) {
                double val = w->text().toDouble();
                if(valueSpinBox.value() > val) {
                    ColorLineEdit *newLine = new ColorLineEdit();
                    newLine->setMode(colorLineEdit.mode());
                    newLine->setColor(colorLineEdit.color());
                    newLine->setAlignment(Qt::AlignCenter);
                    newLine->setReadOnly(true);
                    newLine->setText(QString::number(valueSpinBox.value()));
                    newLine->setAttribute(Qt::WA_TransparentForMouseEvents);


                    QToolButton* btn_edit = new QToolButton();
                    btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                    connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

                    ui->highThresholdsTableWidget->insertRow(rowCount);
                    ui->highThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
                    ui->highThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
                    ui->highThresholdsTableWidget->resizeColumnToContents(0);
                    return;
                }
                if(qFuzzyCompare(valueSpinBox.value(),val)) {
                    w->setMode(colorLineEdit.mode());
                    w->setColor(colorLineEdit.color());
                    w->setText(QString::number(valueSpinBox.value()));

                    return;
                }
            }
        }

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(colorLineEdit.mode());
        newLine->setColor(colorLineEdit.color());
        newLine->setAlignment(Qt::AlignCenter);
        newLine->setReadOnly(true);
        newLine->setText(QString::number(valueSpinBox.value()));
        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);


        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

        ui->highThresholdsTableWidget->insertRow(rowCount);
        ui->highThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
        ui->highThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
        ui->highThresholdsTableWidget->resizeColumnToContents(0);
    }
}

void PropertiesWidget::on_addLowThresholdToolButton_clicked()
{
    QDialog dial;
    dial.setWindowTitle(QString("Add low threshold"));
    QVBoxLayout dialLayout;
    QHBoxLayout layout;
    QVBoxLayout textLayout;
    QVBoxLayout widgetsLayout;

    QLabel valueLabel("Value");
    QDoubleSpinBox valueSpinBox;
    valueSpinBox.setLocale(QLocale("C"));
    valueSpinBox.setRange(-999999999., 999999999.);
    textLayout.addWidget(&valueLabel);
    widgetsLayout.addWidget(&valueSpinBox);


    QLabel colorLabel("Color");
    QHBoxLayout colorLayout;
    QPushButton colorButton;
    colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
    ColorLineEdit colorLineEdit;
    colorLineEdit.setMode(ColorSettings::cmFilledBackground);
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

    QLabel modeLabel("Mode");
    QComboBox modeCombobox;
    modeCombobox.addItem("Filled Background");
    modeCombobox.addItem("No Background");
    modeCombobox.setCurrentIndex(0);

    connect(&modeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&colorLineEdit](int index) {
        colorLineEdit.setMode(ColorSettings::ColorMode(index+1));
    });

    textLayout.addWidget(&modeLabel);
    widgetsLayout.addWidget(&modeCombobox);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

    layout.addLayout(&textLayout);
    layout.addLayout(&widgetsLayout);

    dialLayout.addLayout(&layout);
    dialLayout.addWidget(&buttonBox);

    dial.setLayout(&dialLayout);

    int res = dial.exec();
    if( res == QDialog::Accepted) {
        mThresholdsModified = true;
        int rowCount = 0;
        for(rowCount = 0; rowCount < ui->lowThresholdsTableWidget->rowCount(); rowCount++) {
            ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->lowThresholdsTableWidget->cellWidget(rowCount, 1));
            if(w) {
                double val = w->text().toDouble();
                if(valueSpinBox.value() > val) {
                    ColorLineEdit *newLine = new ColorLineEdit();
                    newLine->setMode(colorLineEdit.mode());
                    newLine->setColor(colorLineEdit.color());
                    newLine->setAlignment(Qt::AlignCenter);
                    newLine->setReadOnly(true);
                    newLine->setText(QString::number(valueSpinBox.value()));
                    newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

                    QToolButton* btn_edit = new QToolButton();
                    btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                    connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

                    ui->lowThresholdsTableWidget->insertRow(rowCount);
                    ui->lowThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
                    ui->lowThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
                    ui->lowThresholdsTableWidget->resizeColumnToContents(0);
                    return;
                }
                if(qFuzzyCompare(valueSpinBox.value(),val)) {
                    w->setMode(colorLineEdit.mode());
                    w->setColor(colorLineEdit.color());
                    w->setText(QString::number(valueSpinBox.value()));

                    return;
                }
            }
        }

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(colorLineEdit.mode());
        newLine->setColor(colorLineEdit.color());
        newLine->setAlignment(Qt::AlignCenter);
        newLine->setReadOnly(true);
        newLine->setText(QString::number(valueSpinBox.value()));
        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

        ui->lowThresholdsTableWidget->insertRow(rowCount);
        ui->lowThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
        ui->lowThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
        ui->lowThresholdsTableWidget->resizeColumnToContents(0);
    }
}

void PropertiesWidget::on_highThresholdsTableWidget_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    ColorLineEdit *lineEdit = qobject_cast<ColorLineEdit *>(ui->highThresholdsTableWidget->cellWidget(row, 1));
    if(lineEdit) {
        QDialog dial;
        dial.setWindowTitle(QString("Edit high threshold"));
        QVBoxLayout dialLayout;
        QHBoxLayout layout;
        QVBoxLayout textLayout;
        QVBoxLayout widgetsLayout;

        QLabel valueLabel("Value");
        QDoubleSpinBox valueSpinBox;
        valueSpinBox.setLocale(QLocale("C"));
        valueSpinBox.setRange(-999999999., 999999999.);
        textLayout.addWidget(&valueLabel);
        widgetsLayout.addWidget(&valueSpinBox);


        QLabel colorLabel("Color");
        QHBoxLayout colorLayout;
        QPushButton colorButton;
        colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
        ColorLineEdit colorLineEdit;
        colorLineEdit.setMode(ColorSettings::cmFilledBackground);
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

        QLabel modeLabel("Mode");
        QComboBox modeCombobox;
        modeCombobox.addItem("Filled Background");
        modeCombobox.addItem("No Background");
        modeCombobox.setCurrentIndex(0);

        connect(&modeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&colorLineEdit](int index) {
            colorLineEdit.setMode(ColorSettings::ColorMode(index+1));
        });


        textLayout.addWidget(&modeLabel);
        widgetsLayout.addWidget(&modeCombobox);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

        connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

        layout.addLayout(&textLayout);
        layout.addLayout(&widgetsLayout);

        dialLayout.addLayout(&layout);
        dialLayout.addWidget(&buttonBox);

        dial.setLayout(&dialLayout);

        auto text = lineEdit->text();
        auto mode = lineEdit->mode();
        auto color = lineEdit->color();

        colorLineEdit.setMode(mode);
        colorLineEdit.setColor(color);
        valueSpinBox.setValue(text.toDouble());
        modeCombobox.setCurrentIndex(mode-1);

        int res = dial.exec();
        if( res == QDialog::Accepted) {
            mThresholdsModified = true;
            int rowCount = 0;
            for(rowCount = 0; rowCount < ui->highThresholdsTableWidget->rowCount(); rowCount++) {
                ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->highThresholdsTableWidget->cellWidget(rowCount, 1));
                if(w) {
                    double val = w->text().toDouble();
                    if(valueSpinBox.value() > val) {
                        ColorLineEdit *newLine = new ColorLineEdit();
                        newLine->setMode(colorLineEdit.mode());
                        newLine->setColor(colorLineEdit.color());
                        newLine->setAlignment(Qt::AlignCenter);
                        newLine->setReadOnly(true);
                        newLine->setText(QString::number(valueSpinBox.value()));
                        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);


                        QToolButton* btn_edit = new QToolButton();
                        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

                        ui->highThresholdsTableWidget->insertRow(rowCount);
                        ui->highThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
                        ui->highThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
                        ui->highThresholdsTableWidget->resizeColumnToContents(0);

                        if(rowCount <= row)
                            row++;
                        ui->highThresholdsTableWidget->removeRow(row);
                        return;
                    }
                    if(qFuzzyCompare(valueSpinBox.value(),val)) {
                        w->setMode(colorLineEdit.mode());
                        w->setColor(colorLineEdit.color());
                        w->setText(QString::number(valueSpinBox.value()));

                        return;
                    }
                }
            }

            ColorLineEdit *newLine = new ColorLineEdit();
            newLine->setMode(colorLineEdit.mode());
            newLine->setColor(colorLineEdit.color());
            newLine->setAlignment(Qt::AlignCenter);
            newLine->setReadOnly(true);
            newLine->setText(QString::number(valueSpinBox.value()));
            newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

            QToolButton* btn_edit = new QToolButton();
            btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
            connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

            ui->highThresholdsTableWidget->insertRow(rowCount);
            ui->highThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
            ui->highThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
            ui->highThresholdsTableWidget->resizeColumnToContents(0);

            ui->highThresholdsTableWidget->removeRow(row);
        }
    }
}

void PropertiesWidget::on_lowThresholdsTableWidget_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    ColorLineEdit *lineEdit = qobject_cast<ColorLineEdit *>(ui->lowThresholdsTableWidget->cellWidget(row, 1));
    if(lineEdit) {
        QDialog dial;
        dial.setWindowTitle(QString("Edit low threshold"));
        QVBoxLayout dialLayout;
        QHBoxLayout layout;
        QVBoxLayout textLayout;
        QVBoxLayout widgetsLayout;

        QLabel valueLabel("Value");
        QDoubleSpinBox valueSpinBox;
        valueSpinBox.setLocale(QLocale("C"));
        valueSpinBox.setRange(-999999999., 999999999.);
        textLayout.addWidget(&valueLabel);
        widgetsLayout.addWidget(&valueSpinBox);

        QLabel colorLabel("Color");
        QHBoxLayout colorLayout;
        QPushButton colorButton;
        colorButton.setIcon(QIcon(QPixmap(":/icons8_paint_palette_50px.png")));
        ColorLineEdit colorLineEdit;
        colorLineEdit.setMode(ColorSettings::cmFilledBackground);
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

        QLabel modeLabel("Mode");
        QComboBox modeCombobox;
        modeCombobox.addItem("Filled Background");
        modeCombobox.addItem("No Background");
        modeCombobox.setCurrentIndex(0);

        connect(&modeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&colorLineEdit](int index) {
            colorLineEdit.setMode(ColorSettings::ColorMode(index+1));
        });

        textLayout.addWidget(&modeLabel);
        widgetsLayout.addWidget(&modeCombobox);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

        connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

        layout.addLayout(&textLayout);
        layout.addLayout(&widgetsLayout);

        dialLayout.addLayout(&layout);
        dialLayout.addWidget(&buttonBox);

        dial.setLayout(&dialLayout);

        auto text = lineEdit->text();
        auto mode = lineEdit->mode();
        auto color = lineEdit->color();

        colorLineEdit.setMode(mode);
        colorLineEdit.setColor(color);
        valueSpinBox.setValue(text.toDouble());
        modeCombobox.setCurrentIndex(mode-1);

        int res = dial.exec();
        if( res == QDialog::Accepted) {
            mThresholdsModified = true;
            int rowCount = 0;
            for(rowCount = 0; rowCount < ui->lowThresholdsTableWidget->rowCount(); rowCount++) {
                ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->lowThresholdsTableWidget->cellWidget(rowCount, 1));
                if(w) {
                    double val = w->text().toDouble();
                    if(valueSpinBox.value() > val) {
                        ColorLineEdit *newLine = new ColorLineEdit();
                        newLine->setMode(colorLineEdit.mode());
                        newLine->setColor(colorLineEdit.color());
                        newLine->setAlignment(Qt::AlignCenter);
                        newLine->setReadOnly(true);
                        newLine->setText(QString::number(valueSpinBox.value()));
                        newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

                        QToolButton* btn_edit = new QToolButton();
                        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

                        ui->lowThresholdsTableWidget->insertRow(rowCount);
                        ui->lowThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
                        ui->lowThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
                        ui->lowThresholdsTableWidget->resizeColumnToContents(0);

                        if(rowCount <= row)
                            row++;
                        ui->lowThresholdsTableWidget->removeRow(row);
                        return;
                    }
                    if(qFuzzyCompare(valueSpinBox.value(),val)) {
                        w->setMode(colorLineEdit.mode());
                        w->setColor(colorLineEdit.color());
                        w->setText(QString::number(valueSpinBox.value()));

                        return;
                    }
                }
            }

            ColorLineEdit *newLine = new ColorLineEdit();
            newLine->setMode(colorLineEdit.mode());
            newLine->setColor(colorLineEdit.color());
            newLine->setAlignment(Qt::AlignCenter);
            newLine->setReadOnly(true);
            newLine->setText(QString::number(valueSpinBox.value()));
            newLine->setAttribute(Qt::WA_TransparentForMouseEvents);

            QToolButton* btn_edit = new QToolButton();
            btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
            connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

            ui->lowThresholdsTableWidget->insertRow(rowCount);
            ui->lowThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
            ui->lowThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
            ui->lowThresholdsTableWidget->resizeColumnToContents(0);

            ui->lowThresholdsTableWidget->removeRow(row);
        }
    }
}

QExplicitlySharedDataPointer<ParameterConfiguration> PropertiesWidget::currentSettings() const
{
    return mCurrentSettings;
}

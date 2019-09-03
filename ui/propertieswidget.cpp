#include "propertieswidget.h"
#include "ui_propertieswidget.h"
#include <QColorDialog>

PropertiesWidget::PropertiesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertiesWidget),
    mStatesModified(false),
    mThresholdsModified(false),
    mBitfieldsModified(false)
{
    ui->setupUi(this);
    ui->propertiesStackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    ui->stateTextColorLineEdit->setColorText(false);
}

PropertiesWidget::~PropertiesWidget()
{
    delete ui;
}

bool PropertiesWidget::isConnected()
{
    return ui->connectedCheckBox->isChecked();
}

void PropertiesWidget::updateUi(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
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
    ui->colorModeComboBox->setCurrentIndex(parameterSettings->defaultColorSettingsRef().mode());

    ui->outOfRangeCheckBox->setChecked(parameterSettings->outOfRangeColorEnabled());
    ui->outOfRangeColorLineEdit->setColor(parameterSettings->outOfRangeColorSettingsRef().color());
    ui->outOfRangeColorLineEdit->setMode(parameterSettings->outOfRangeColorSettingsRef().mode());
    ui->outOfRangeColorModeComboBox->setCurrentIndex(parameterSettings->outOfRangeColorSettingsRef().mode());

    ui->lowThresholdsTableWidget->clear();
    QMapIterator<double, QTBColorSettings> itLow(parameterSettings->thresholdsSettingsRef().lowThresholds());
    while (itLow.hasNext()) {
        itLow.next();

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(itLow.value().mode());
        newLine->setColor(itLow.value().color());
        newLine->setAlignment(Qt::AlignCenter);
        newLine->setText(QString::number(itLow.key()));

        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

        ui->lowThresholdsTableWidget->insertRow(0);
        ui->lowThresholdsTableWidget->setCellWidget(0,0,btn_edit);
        ui->lowThresholdsTableWidget->setCellWidget(0,1,newLine);
    }
    ui->lowThresholdsTableWidget->resizeColumnToContents(0);

    ui->highThresholdsTableWidget->clear();
    QMapIterator<double, QTBColorSettings> itHigh(parameterSettings->thresholdsSettingsRef().highThresholds());
    while (itHigh.hasNext()) {
        itHigh.next();

        ColorLineEdit *newLine = new ColorLineEdit();
        newLine->setMode(itHigh.value().mode());
        newLine->setColor(itHigh.value().color());
        newLine->setAlignment(Qt::AlignCenter);
        newLine->setText(QString::number(itHigh.key()));

        QToolButton* btn_edit = new QToolButton();
        btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
        connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

        ui->highThresholdsTableWidget->insertRow(0);
        ui->highThresholdsTableWidget->setCellWidget(0,0,btn_edit);
        ui->highThresholdsTableWidget->setCellWidget(0,1,newLine);
    }
    ui->highThresholdsTableWidget->resizeColumnToContents(0);

    ui->statesTableWidget->clear();
    QMapIterator<qlonglong, QTBColorSettings> itStateColor(parameterSettings->statesSettingsRef().statesColor());
    while (itStateColor.hasNext()) {
        itStateColor.next();

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(itStateColor.key()));
        item->setTextAlignment(Qt::AlignCenter);

        ColorLineEdit *newLine2 = new ColorLineEdit();
        newLine2->setMode(itStateColor.value().mode());
        newLine2->setColor(itStateColor.value().color());
        newLine2->setAlignment(Qt::AlignLeft);
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

    ui->graphBrushCheckBox->setChecked(parameterSettings->graphBrush());
    ui->graphLineStyleComboBox->setCurrentIndex(parameterSettings->graphLineStyle());
    ui->itemsThresholdsCheckBox->setChecked(parameterSettings->itemsThresholdsVisible());

    ui->curveLineStyleComboBox->setCurrentIndex(parameterSettings->curveLineStyle());
    ui->curveTracerCheckBox->setChecked(parameterSettings->curveTracerVisible());
}

void PropertiesWidget::updateParameterSettings(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
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
    parameterSettings->defaultColorSettingsRef().setMode(QTBColorSettings::ColorMode(ui->colorModeComboBox->currentIndex()));

    parameterSettings->setOutOfRangeColorEnabled(ui->outOfRangeCheckBox->isChecked());
    parameterSettings->outOfRangeColorSettingsRef().setColor(ui->outOfRangeColorLineEdit->color());
    parameterSettings->outOfRangeColorSettingsRef().setMode(ui->outOfRangeColorLineEdit->mode());

    if(mThresholdsModified) {
        parameterSettings->thresholdsSettingsRef().clearHighThreshold();
        for(int i=0; i<ui->highThresholdsTableWidget->rowCount();i++) {
            ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->highThresholdsTableWidget->cellWidget(i, 1));
            if(w) {
                double val = w->text().toDouble();
                QTBColorSettings cs;
                cs.setMode(w->mode());
                cs.setColor(w->color());
                parameterSettings->thresholdsSettingsRef().insertHighThreshold(val, cs);
            }
        }

        parameterSettings->thresholdsSettingsRef().clearLowThreshold();
        for(int i=0; i<ui->lowThresholdsTableWidget->rowCount();i++) {
            ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->lowThresholdsTableWidget->cellWidget(i, 1));
            if(w) {
                double val = w->text().toDouble();
                QTBColorSettings cs;
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

                QTBColorSettings cs;
                cs.setMode(cl->mode());
                cs.setColor(cl->color());

                parameterSettings->statesSettingsRef().addState(val, cl->text(), cs);
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

    parameterSettings->setGraphBrush(ui->graphBrushCheckBox->isChecked());
    parameterSettings->setGraphLineStyle(QCPGraph::LineStyle(ui->graphLineStyleComboBox->currentIndex()));
    parameterSettings->setItemsThresholdsVisible(ui->itemsThresholdsCheckBox->isChecked());

    parameterSettings->setCurveLineStyle(QCPCurve::LineStyle(ui->curveLineStyleComboBox->currentIndex()));
    parameterSettings->setCurveTracerVisible(ui->curveTracerCheckBox->isChecked());
}

void PropertiesWidget::on_colorModeComboBox_currentIndexChanged(int index)
{
    ui->colorLineEdit->setMode(QTBColorSettings::ColorMode(index));
}

void PropertiesWidget::on_defaultColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->colorLineEdit->color());
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
    QColor color = QColorDialog::getColor(ui->outOfRangeColorLineEdit->color());
    if(color.isValid()) {
        ui->outOfRangeColorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_outOfRangeColorModeComboBox_currentIndexChanged(int index)
{
    ui->outOfRangeColorLineEdit->setMode(QTBColorSettings::ColorMode(index));
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
    }   else {
        emit connectProperties(false);
    }
}

void PropertiesWidget::setEditionMode(const EditionMode &mode)
{
    mEditionMode = mode;

    switch(mEditionMode) {
    case emCreation:
        ui->connectedCheckBox->setChecked(true);
        ui->connectedWidget->setVisible(true);
        ui->connectedWidget->setEnabled(false);
        ui->descriptionWidget->setEnabled(true);
        ui->descriptionLineEdit->setEnabled(true);
        ui->labelWidget->setEnabled(true);
        ui->labelLineEdit->setEnabled(true);
        ui->userWidget->setEnabled(true);
        break;
    case emEdition:
        ui->connectedCheckBox->setChecked(true);
        ui->connectedWidget->setVisible(true);
        ui->connectedWidget->setEnabled(false);
        ui->descriptionWidget->setEnabled(false);
        ui->descriptionLineEdit->setEnabled(false);
        ui->labelWidget->setEnabled(false);
        ui->labelLineEdit->setEnabled(false);
        ui->userWidget->setEnabled(true);
        break;
    case emElementConnected:
        ui->connectedCheckBox->setChecked(true);
        ui->connectedWidget->setVisible(true);
        ui->connectedWidget->setEnabled(true);
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
        ui->connectedWidget->setVisible(true);
        ui->connectedWidget->setEnabled(true);
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
        ui->connectedWidget->setVisible(false);
        ui->connectedWidget->setEnabled(false);
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

void PropertiesWidget::on_highThresholdPushButton_clicked()
{    
    mThresholdsModified = true;
    int rowCount = 0;
    for(rowCount = 0; rowCount < ui->highThresholdsTableWidget->rowCount(); rowCount++) {
        ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->highThresholdsTableWidget->cellWidget(rowCount, 1));
        if(w) {
            double val = w->text().toDouble();
            if(ui->highThresholdDoubleSpinBox->value() > val) {
                ColorLineEdit *newLine = new ColorLineEdit();
                newLine->setMode(ui->highThresholdColorLineEdit->mode());
                newLine->setColor(ui->highThresholdColorLineEdit->color());
                newLine->setAlignment(Qt::AlignCenter);
                newLine->setText(QString::number(ui->highThresholdDoubleSpinBox->value()));

                QToolButton* btn_edit = new QToolButton();
                btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

                ui->highThresholdsTableWidget->insertRow(rowCount);
                ui->highThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
                ui->highThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
                ui->highThresholdsTableWidget->resizeColumnToContents(0);
                return;
            }
            if(qFuzzyCompare(ui->highThresholdDoubleSpinBox->value(),val)) {
                w->setMode(ui->highThresholdColorLineEdit->mode());
                w->setColor(ui->highThresholdColorLineEdit->color());
                w->setText(QString::number(ui->highThresholdDoubleSpinBox->value()));

                return;
            }
        }
    }

    ColorLineEdit *newLine = new ColorLineEdit();
    newLine->setMode(ui->highThresholdColorLineEdit->mode());
    newLine->setColor(ui->highThresholdColorLineEdit->color());
    newLine->setAlignment(Qt::AlignCenter);
    newLine->setText(QString::number(ui->highThresholdDoubleSpinBox->value()));

    QToolButton* btn_edit = new QToolButton();
    btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
    connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeHighThreshold_clicked);

    ui->highThresholdsTableWidget->insertRow(rowCount);
    ui->highThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
    ui->highThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
    ui->highThresholdsTableWidget->resizeColumnToContents(0);

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

void PropertiesWidget::on_highTthresholdColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->highThresholdColorLineEdit->color());
    if(color.isValid()) {
        ui->highThresholdColorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_highThresholdColorModeComboBox_currentIndexChanged(int index)
{
    ui->highThresholdColorLineEdit->setMode(QTBColorSettings::ColorMode(index));
}

void PropertiesWidget::on_lowThresholdColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->lowThresholdColorLineEdit->color());
    if(color.isValid()) {
        ui->lowThresholdColorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_lowThresholdColorModeComboBox_currentIndexChanged(int index)
{
    ui->lowThresholdColorLineEdit->setMode(QTBColorSettings::ColorMode(index));
}

void PropertiesWidget::on_lowThresholdPushButton_clicked()
{
    mThresholdsModified = true;
    int rowCount = 0;
    for(rowCount = 0; rowCount < ui->lowThresholdsTableWidget->rowCount(); rowCount++) {
        ColorLineEdit *w = qobject_cast<ColorLineEdit *>(ui->lowThresholdsTableWidget->cellWidget(rowCount, 1));
        if(w) {
            double val = w->text().toDouble();
            if(ui->lowThresholdDoubleSpinBox->value() > val) {
                ColorLineEdit *newLine = new ColorLineEdit();
                newLine->setMode(ui->lowThresholdColorLineEdit->mode());
                newLine->setColor(ui->lowThresholdColorLineEdit->color());
                newLine->setAlignment(Qt::AlignCenter);
                newLine->setText(QString::number(ui->lowThresholdDoubleSpinBox->value()));

                QToolButton* btn_edit = new QToolButton();
                btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
                connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

                ui->lowThresholdsTableWidget->insertRow(rowCount);
                ui->lowThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
                ui->lowThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
                ui->lowThresholdsTableWidget->resizeColumnToContents(0);
                return;
            }
            if(qFuzzyCompare(ui->lowThresholdDoubleSpinBox->value(),val)) {
                w->setMode(ui->lowThresholdColorLineEdit->mode());
                w->setColor(ui->lowThresholdColorLineEdit->color());
                w->setText(QString::number(ui->lowThresholdDoubleSpinBox->value()));

                return;
            }
        }
    }

    ColorLineEdit *newLine = new ColorLineEdit();
    newLine->setMode(ui->lowThresholdColorLineEdit->mode());
    newLine->setColor(ui->lowThresholdColorLineEdit->color());
    newLine->setAlignment(Qt::AlignCenter);
    newLine->setText(QString::number(ui->lowThresholdDoubleSpinBox->value()));

    QToolButton* btn_edit = new QToolButton();
    btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
    connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeLowThreshold_clicked);

    ui->lowThresholdsTableWidget->insertRow(rowCount);
    ui->lowThresholdsTableWidget->setCellWidget(rowCount,0,btn_edit);
    ui->lowThresholdsTableWidget->setCellWidget(rowCount,1,newLine);
    ui->lowThresholdsTableWidget->resizeColumnToContents(0);
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

void PropertiesWidget::on_addStatePushButton_clicked()
{
    mStatesModified = true;
    int rowCount = 0;
    for(rowCount = 0; rowCount < ui->statesTableWidget->rowCount(); rowCount++) {
        ColorLineEdit *cl = qobject_cast<ColorLineEdit *>(ui->statesTableWidget->cellWidget(rowCount, 2));
        if(cl) {
            qlonglong val = ui->statesTableWidget->item(rowCount,1)->text().toLongLong();
            if(ui->stateSpinBox->value() > val) {
                QTableWidgetItem *item = new QTableWidgetItem(QString::number(ui->stateSpinBox->value()));
                item->setTextAlignment(Qt::AlignCenter);

                ColorLineEdit *newLine = new ColorLineEdit();
                newLine->setMode(ui->stateColorLineEdit->mode());
                newLine->setColor(ui->stateColorLineEdit->color());
                newLine->setAlignment(Qt::AlignLeft);
                newLine->setText(ui->stateTextColorLineEdit->text());

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
            if(ui->stateSpinBox->value() == val) {
                ui->statesTableWidget->item(rowCount,1)->setText(QString::number(ui->stateSpinBox->value()));

                cl->setMode(ui->stateColorLineEdit->mode());
                cl->setColor(ui->stateColorLineEdit->color());
                cl->setText(ui->stateTextColorLineEdit->text());

                return;
            }
        }
    }

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(ui->stateSpinBox->value()));
    item->setTextAlignment(Qt::AlignCenter);

    ColorLineEdit *newLine = new ColorLineEdit();
    newLine->setMode(ui->stateColorLineEdit->mode());
    newLine->setColor(ui->stateColorLineEdit->color());
    newLine->setAlignment(Qt::AlignLeft);
    newLine->setText(ui->stateTextColorLineEdit->text());

    QToolButton* btn_edit = new QToolButton();
    btn_edit->setIcon(QIcon(QPixmap(":/icons8_cancel_50px.png")));
    connect(btn_edit, &QToolButton::clicked, this, &PropertiesWidget::removeState_clicked);

    ui->statesTableWidget->insertRow(rowCount);
    ui->statesTableWidget->setCellWidget(rowCount,0,btn_edit);
    ui->statesTableWidget->setItem(rowCount,1,item);
    ui->statesTableWidget->setCellWidget(rowCount,2,newLine);
    ui->statesTableWidget->resizeColumnToContents(0);

}

void PropertiesWidget::on_stateColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->stateColorLineEdit->color());
    if(color.isValid()) {
        ui->stateColorLineEdit->setColor(color);
    }
}

void PropertiesWidget::on_stateColorModeComboBox_currentIndexChanged(int index)
{
    ui->stateColorLineEdit->setMode(QTBColorSettings::ColorMode(index));
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

void PropertiesWidget::setPropertiesMode(const QTBParameterConfiguration::ConfigurationModule &propertiesMode)
{
    mPropertiesMode = propertiesMode;

    ui->listWidget->clear();
    ui->listWidget->addItem("General");
    ui->listWidget->addItem("Style");
    switch(mPropertiesMode) {
    case QTBParameterConfiguration::cmFull:
        ui->listWidget->addItem("Thresholds");
        ui->listWidget->addItem("States");
        ui->listWidget->addItem("Bitfields");
        ui->itemsStyleWidget->setVisible(true);
        break;
    case QTBParameterConfiguration::cmValue:
        ui->listWidget->addItem("Thresholds");
        ui->itemsStyleWidget->setVisible(false);
        break;
    case QTBParameterConfiguration::cmState:
        ui->listWidget->addItem("States");
        ui->itemsStyleWidget->setVisible(false);
        break;
    case QTBParameterConfiguration::cmCurve:
        ui->listWidget->addItem("Thresholds");
        ui->itemsStyleWidget->setVisible(true);
        break;
    case QTBParameterConfiguration::cmBitFields:
        ui->listWidget->addItem("Bitfields");
        ui->itemsStyleWidget->setVisible(false);
        break;
    }
}

void PropertiesWidget::on_itemsColorComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->itemsColorWidget->setEnabled(true);
    else
        ui->itemsColorWidget->setEnabled(false);
}

void PropertiesWidget::on_itemsColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->itemsColorLineEdit->color());
    if(color.isValid()) {
        ui->itemsColorLineEdit->setColor(color);
    }
}

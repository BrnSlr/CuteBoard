#include "table_values_editor.h"
#include "ui_table_values_editor.h"

TableValuesEditor::TableValuesEditor(TableValues *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableValuesEditor),
    mElement(display)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &TableValuesEditor::updateElement);


    if(mElement) {
        ui->backgroundComboBox->setCurrentIndex(mElement->backgroundMode());
        ui->unitVisibleCheckBox->setChecked(mElement->unitVisible());
        ui->titleVisibleCheckBox->setChecked(mElement->titleVisible());
        ui->titleLineEdit->setText(mElement->title());
        ui->alternatingRowColorsCheckBox->setChecked(mElement->alternatingRowColors());
        ui->valueFormatComboBox->setCurrentIndex(mElement->valueFormat());
        ui->precisionSpinBox->setValue(mElement->precision());
    }
    setMinimumSize(600,400);
}

TableValuesEditor::~TableValuesEditor()
{
    delete ui;
}

void TableValuesEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void TableValuesEditor::updateElement()
{
    if(mElement) {
        mElement->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        mElement->setUnitVisible(ui->unitVisibleCheckBox->isChecked());
        mElement->setTitleVisible(ui->titleVisibleCheckBox->isChecked());
        mElement->setTitle(ui->titleLineEdit->text());
        mElement->setAlternatingRowColors(ui->alternatingRowColorsCheckBox->isChecked());
        mElement->setValueFormat(ValueStringFormat(ui->valueFormatComboBox->currentIndex()));
        mElement->setPrecision(ui->precisionSpinBox->value());
        mElement->rebuildElement(true);
    }
}

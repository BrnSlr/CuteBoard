#include "text_label_editor.h"
#include "ui_text_label_editor.h"

TextLabelEditor::TextLabelEditor(TextLabel *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextLabelEditor),
    mElement(display)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &TextLabelEditor::updateElement);

    if(mElement) {
        ui->backgroundComboBox->setCurrentIndex(mElement->backgroundMode());
        ui->textLineEdit->setText(mElement->text());
        ui->colorLineEdit->setColor(mElement->colorSettingsRef().color());
        ui->colorLineEdit->setMode(mElement->colorSettingsRef().mode());
        ui->colorModeComboBox->setCurrentIndex(mElement->colorSettingsRef().mode()-1);
    }
    setMinimumSize(600,400);
}

TextLabelEditor::~TextLabelEditor()
{
    delete ui;
}

void TextLabelEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void TextLabelEditor::updateElement()
{
    if(mElement) {
        mElement->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        mElement->setText(ui->textLineEdit->text());
        mElement->colorSettingsRef().setColor(ui->colorLineEdit->color());
        mElement->colorSettingsRef().setMode(ui->colorLineEdit->mode());
        mElement->rebuildElement(true);
    }
}

void TextLabelEditor::on_colorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->colorLineEdit->color(), this, QString("Color picker"), QColorDialog::ShowAlphaChannel);
    if(color.isValid()) {
        ui->colorLineEdit->setColor(color);
    }
}

void TextLabelEditor::on_colorModeComboBox_currentIndexChanged(int index)
{
    ui->colorLineEdit->setMode(ColorSettings::ColorMode(index+1));
}

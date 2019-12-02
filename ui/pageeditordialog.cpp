#include "pageeditordialog.h"
#include "ui_pageeditordialog.h"

PageEditorDialog::PageEditorDialog(bool editPage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PageEditorDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(!editPage);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->backgroundLineEdit, SIGNAL(textEdited(QString)), this, SLOT(loadImage()));
}

PageEditorDialog::~PageEditorDialog()
{
    delete ui;
}

QString PageEditorDialog::getName()
{
    return ui->lineEdit->text();
}

int PageEditorDialog::getColumnCount()
{
    return ui->columnsSpinBox->value();
}

int PageEditorDialog::getRowCount()
{
    return ui->rowsSpinBox->value();
}

int PageEditorDialog::getSingleElementColumnCount()
{
    return ui->singleColumnsSpinBox->value();
}

int PageEditorDialog::getSingleElementRowCount()
{
    return ui->singleRowsSpinBox->value();
}

QString PageEditorDialog::getDescription()
{
    QString descr = ui->textEdit->toPlainText();
    return descr;
}

bool PageEditorDialog::getCustomLayout()
{
    return ui->checkBox->isChecked();
}

QString PageEditorDialog::getBackground()
{
    return ui->backgroundLineEdit->text();
}

void PageEditorDialog::setName(const QString& name)
{
    ui->lineEdit->setText(name);
}

void PageEditorDialog::setBackground(const QString& file)
{
    ui->backgroundLineEdit->setText(file);
}

void PageEditorDialog::setDescription(const QString& descr)
{
    ui->textEdit->setPlainText(descr);
}

void PageEditorDialog::on_densitySpinBox_valueChanged(int arg1)
{
    ui->rowsSpinBox->setValue(arg1 * DEFAULT_NB_ROWS);
    ui->columnsSpinBox->setValue(arg1 * DEFAULT_NB_COLUMNS);
    ui->singleRowsSpinBox->setValue(arg1);
    ui->singleColumnsSpinBox->setValue(arg1);
}

void PageEditorDialog::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) {
        ui->singleWidget->setEnabled(true);
    } else {
        on_densitySpinBox_valueChanged(ui->densitySpinBox->value());
        ui->singleWidget->setEnabled(false);
    }
}

void PageEditorDialog::loadImage()
{
    if(!ui->backgroundLineEdit->text().isEmpty()) {
        QPixmap pic(ui->backgroundLineEdit->text());

        // get label dimensions
        int w = ui->backgroundPreview->width();
        int h = ui->backgroundPreview->height();

        // set a scaled pixmap to a w x h window keeping its aspect ratio
        ui->backgroundPreview->setPixmap(pic.scaled(w,h,Qt::KeepAspectRatio));
    }
}


void PageEditorDialog::setColumnCount(int column)
{
    ui->columnsSpinBox->setValue(column);
}

void PageEditorDialog::setRowCount(int row)
{
    ui->rowsSpinBox->setValue(row);
}

void PageEditorDialog::setSingleElementColumnCount(int column)
{
    ui->singleColumnsSpinBox->setValue(column);
}

void PageEditorDialog::setSingleElementRowCount(int row)
{
    ui->singleRowsSpinBox->setValue(row);
}

void PageEditorDialog::setCustomLayout(bool custom)
{
    ui->checkBox->setChecked(custom);
    if(!custom)
        ui->densitySpinBox->setValue(ui->rowsSpinBox->value() / DEFAULT_NB_ROWS);
}

void PageEditorDialog::on_backgroundPushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if(!fileName.isEmpty()) {
        ui->backgroundLineEdit->setText(fileName);
        loadImage();
    }
}

void PageEditorDialog::resizeEvent(QResizeEvent *ev)
{
    QDialog::resizeEvent(ev);
    QPixmap pic( ui->backgroundLineEdit->text());

    // get label dimensions
    int w = ui->backgroundPreview->width();
    int h = ui->backgroundPreview->height();

    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->backgroundPreview->setPixmap(pic.scaled(w,h,Qt::KeepAspectRatio));
}

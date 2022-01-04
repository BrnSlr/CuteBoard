#include "page_editor_dialog.h"
#include "ui_page_editor_dialog.h"

PageEditorDialog::PageEditorDialog(bool editPage, QSharedPointer<Project> project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PageEditorDialog),
    mProject(project)
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(!editPage);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->backgroundLineEdit, SIGNAL(textEdited(QString)), this, SLOT(loadImage()));
    loadImage();
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

void PageEditorDialog::loadImage()
{
    if(!ui->backgroundLineEdit->text().isEmpty()) {

        QPixmap pic(mProject->resourcesImagesPath() + QDir::separator() + ui->backgroundLineEdit->text());

        // get label dimensions
        int w = ui->backgroundLineEdit->width();

        //        int w = ui->backgroundPreview->width();
        //        int h = ui->backgroundPreview->height();

        // set a scaled pixmap to a w x h window keeping its aspect ratio
        ui->backgroundPreview->setPixmap(pic.scaledToWidth(w,Qt::FastTransformation));

        //        ui->backgroundPreview->setPixmap(pic);
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

void PageEditorDialog::on_backgroundPushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), mProject->resourcesImagesPath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if(!fileName.isEmpty()) {

        auto fi = QFileInfo(fileName);

        QString imagePath = mProject->resourcesImagesPath() + QDir::separator() + fi.fileName();

        if(QFile::exists(imagePath)) {
            QFile::remove(imagePath);
        }

        if(QFile::copy(fileName, imagePath)) {
            ui->backgroundLineEdit->setText(fi.fileName());
            loadImage();
        } else {
            QMessageBox::warning(this, "Background image", QString("Error : can't copy file to project directory\n%1").arg(mProject->resourcesImagesPath()));
            ui->backgroundLineEdit->setText("");
        }
    }
}

void PageEditorDialog::resizeEvent(QResizeEvent *ev)
{
    QDialog::resizeEvent(ev);
    QPixmap pic(mProject->resourcesImagesPath() + QDir::separator() + ui->backgroundLineEdit->text());

    // get label dimensions
    int w = ui->backgroundLineEdit->width();

    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->backgroundPreview->setPixmap(pic.scaledToWidth(w,Qt::FastTransformation));
}

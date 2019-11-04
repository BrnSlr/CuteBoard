#include "curves_xy_patron_editor.h"
#include "ui_curves_xy_patron_editor.h"

CurvesXYPatronEditor::CurvesXYPatronEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurvesXYPatronEditor)
{
    ui->setupUi(this);
    ui->colorLineEdit->setEnabled(false);
}

CurvesXYPatronEditor::~CurvesXYPatronEditor()
{
    delete ui;
}

void CurvesXYPatronEditor::on_importPushButton_clicked()
{

}

void CurvesXYPatronEditor::on_exportPushButton_clicked()
{

}

bool CurvesXYPatronEditor::checkName()
{
    if(ui->nameLineEdit->text().isEmpty())
        return false;
    else
        return true;
}

void CurvesXYPatronEditor::loadPatron(QSharedPointer<QTBCurvePatronConfiguration> patron)
{
    mPatron = patron;
    ui->nameLineEdit->setText(patron->name());
    ui->colorLineEdit->setColor(patron->color());
    ui->lineStyleComboBox->setCurrentIndex(int(patron->penStyle())-1);

    QList<double> x,y;
    x = patron->xValues();
    y= patron->yValues();

    ui->tableWidget->clearContents();
    for(int i=0; i< x.count();i++) {

        QTableWidgetItem *itemX = new QTableWidgetItem(QString::number(x.at(i)));
        QTableWidgetItem *itemY = new QTableWidgetItem(QString::number(y.at(i)));

        ui->tableWidget->setItem(i,0,itemX);
        ui->tableWidget->setItem(i,1,itemY);
    }
}

QSharedPointer<QTBCurvePatronConfiguration> CurvesXYPatronEditor::createPatron()
{
    QSharedPointer<QTBCurvePatronConfiguration> patron(new QTBCurvePatronConfiguration());
    patron->setName(ui->nameLineEdit->text());
    patron->setColor(ui->colorLineEdit->color());
    patron->setPenStyle(Qt::PenStyle(ui->lineStyleComboBox->currentIndex() + 1));

    for(int i=0; i< ui->tableWidget->rowCount();i++) {
        double x,y;
        bool xOk=false, yOk=false;
        if(ui->tableWidget->item(i,0))
            x = ui->tableWidget->item(i,0)->text().toDouble(&xOk);
        if(ui->tableWidget->item(i,1))
            y = ui->tableWidget->item(i,1)->text().toDouble(&yOk);

        if(xOk && yOk)
            patron->addPoint(x,y);
    }

    return patron;
}

void CurvesXYPatronEditor::updatePatron()
{
    mPatron->setName(ui->nameLineEdit->text());
    mPatron->setColor(ui->colorLineEdit->color());
    mPatron->setPenStyle(Qt::PenStyle(ui->lineStyleComboBox->currentIndex() + 1));

    mPatron->clearPoints();
    for(int i=0; i< ui->tableWidget->rowCount();i++) {
        double x,y;
        bool xOk=false, yOk=false;
        if(ui->tableWidget->item(i,0))
            x = ui->tableWidget->item(i,0)->text().toDouble(&xOk);
        if(ui->tableWidget->item(i,1))
            y = ui->tableWidget->item(i,1)->text().toDouble(&yOk);

        if(xOk && yOk)
            mPatron->addPoint(x,y);
    }
}

void CurvesXYPatronEditor::on_colorPushButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->colorLineEdit->color());
    if(color.isValid()) {
        ui->colorLineEdit->setColor(color);
    }
}

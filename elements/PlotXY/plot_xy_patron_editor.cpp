#include "plot_xy_patron_editor.h"
#include "ui_plot_xy_patron_editor.h"
#include "csv.h"
#include <QFileDialog>

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
    auto file = QFileDialog::getOpenFileName(this,
                                             "Select PATRON file",
                                             QDir::homePath(),
                                             "PATRON (*.csv)");

    if(!file.isEmpty()) {
        QFileInfo fi(file);
        ui->nameLineEdit->setText(fi.baseName());
        io::CSVReader<2,
                io::trim_chars<' '>,
                io::no_quote_escape<';'>,
                io::ignore_overflow,
                io::single_and_empty_line_comment<'#',';'>> in(file.toStdString());

        std::string key, value;
        int index = 0;
        ui->tableWidget->clearContents();

        while(in.read_row(key,
                          value)) {

            auto qKey = QString::fromStdString(key).replace(',','.');
            auto qValue = QString::fromStdString(value).replace(',','.');

            if(!qKey.isEmpty() && !qValue.isEmpty()) {

                if(index >= ui->tableWidget->rowCount())
                    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

                QTableWidgetItem *itemX = new QTableWidgetItem(qKey);
                QTableWidgetItem *itemY = new QTableWidgetItem(qValue);

                ui->tableWidget->setItem(index,0,itemX);
                ui->tableWidget->setItem(index,1,itemY);
                index++;
            }
        }

    }
}

void CurvesXYPatronEditor::on_exportPushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName (this, "Export to csv file",
                                                     QDir::homePath(), "CSV files (*.csv)");

    if(!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QIODevice::WriteOnly) && !fileName.isNull()) {

            QTextStream stream(&file);

            for(int i=0; i< ui->tableWidget->rowCount();i++) {
                QString key, value;
                if(ui->tableWidget->item(i,0))
                    key = ui->tableWidget->item(i,0)->text();
                if(ui->tableWidget->item(i,1))
                    value = ui->tableWidget->item(i,1)->text();

                if(!key.isEmpty() && !value.isEmpty())
                    stream << key << ";" << value << "\n";
            }

            file.close();
        }
    }

}

bool CurvesXYPatronEditor::checkName()
{
    if(ui->nameLineEdit->text().isEmpty())
        return false;
    else
        return true;
}

void CurvesXYPatronEditor::loadPatron(QSharedPointer<CurvePatron> patron)
{
    mPatron = patron;
    ui->nameLineEdit->setText(patron->name());
    ui->colorLineEdit->setColor(patron->color());
    ui->lineStyleComboBox->setCurrentIndex(int(patron->penStyle())-1);
    ui->lineWidhtSpinBox->setValue(patron->penWidth());

    QList<double> x,y;
    x = patron->xValues();
    y= patron->yValues();

    ui->tableWidget->clearContents();
    for(int i=0; i< x.count();i++) {

        if(i >= ui->tableWidget->rowCount())
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QTableWidgetItem *itemX = new QTableWidgetItem(QString::number(x.at(i)));
        QTableWidgetItem *itemY = new QTableWidgetItem(QString::number(y.at(i)));

        ui->tableWidget->setItem(i,0,itemX);
        ui->tableWidget->setItem(i,1,itemY);
    }
}

QSharedPointer<CurvePatron> CurvesXYPatronEditor::createPatron()
{
    QSharedPointer<CurvePatron> patron(new CurvePatron());
    patron->setName(ui->nameLineEdit->text());
    patron->setColor(ui->colorLineEdit->color());
    patron->setPenStyle(Qt::PenStyle(ui->lineStyleComboBox->currentIndex() + 1));
    patron->setPenWidth(ui->lineWidhtSpinBox->value());

    for(int i=0; i< ui->tableWidget->rowCount();i++) {
        double x = 0.0;
        double y = 0.0;
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
    mPatron->setPenWidth(ui->lineWidhtSpinBox->value());

    mPatron->clearPoints();
    for(int i=0; i< ui->tableWidget->rowCount();i++) {
        double x = 0.0;
        double y = 0.0;
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
    QColor color = QColorDialog::getColor(ui->colorLineEdit->color(), this, QString("Color picker"), QColorDialog::ShowAlphaChannel);
    if(color.isValid()) {
        ui->colorLineEdit->setColor(color);
    }
}

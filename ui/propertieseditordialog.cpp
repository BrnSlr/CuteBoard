#include "propertieseditordialog.h"
#include "ui_propertieseditordialog.h"
#include <QMessageBox>
#include <utility>

PropertiesEditorDialog::PropertiesEditorDialog(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings,
                                                             bool create,
                                                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesEditorDialog),
    mWidget(nullptr),
    mParameterSettings(std::move(parameterSettings))
{
    ui->setupUi(this);
    mWidget = new PropertiesWidget(this);
    if(create)
        mWidget->setEditionMode(PropertiesWidget::emCreation);
    else
        mWidget->setEditionMode(PropertiesWidget::emEdition);
    mWidget->setPropertiesMode(QTBParameterConfiguration::cmFull);
    mWidget->updateUi(mParameterSettings);
    ui->verticalLayout->addWidget(mWidget);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

PropertiesEditorDialog::~PropertiesEditorDialog()
{
    delete ui;
}

void PropertiesEditorDialog::accept()
{
    mWidget->updateParameterSettings(mParameterSettings);
    if(!mParameterSettings->label().isEmpty() && !mParameterSettings->description().isEmpty()) {
        QDialog::accept();
    } else {
        QMessageBox::warning(this, "Visu Parameter Creation", "Error : incomplete parameter (label/descr)");
        return;
    }
}

void PropertiesEditorDialog::on_buttonBox_accepted()
{

}

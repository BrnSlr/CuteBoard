#ifndef PROPERTIESEDITORDIALOG_H
#define PROPERTIESEDITORDIALOG_H

#include <QDialog>
#include "propertieswidget.h"
#include "project/parameter_configuration.h"

namespace Ui {
class PropertiesEditorDialog;
}

class PropertiesEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesEditorDialog(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings,
                                           bool create,
                                           QWidget *parent = nullptr);
    ~PropertiesEditorDialog() Q_DECL_OVERRIDE;

private slots:
    virtual void accept() Q_DECL_OVERRIDE;
    void on_buttonBox_accepted();

private:
    Ui::PropertiesEditorDialog *ui;
    PropertiesWidget *mWidget;
    QExplicitlySharedDataPointer<QTBParameterConfiguration> mParameterSettings;
};

#endif

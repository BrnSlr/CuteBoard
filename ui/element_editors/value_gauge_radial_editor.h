#ifndef CGAUGEEDITOR_H
#define CGAUGEEDITOR_H

#include <QDialog>
#include "dashboard/elements/value_gauge_radial.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBValueGaugeRadialEditor;
}

class QTBValueGaugeRadialEditor : public QDialog
{
    Q_OBJECT

public:

    explicit QTBValueGaugeRadialEditor(QTBValueGaugeRadial *display, QWidget *parent = nullptr);
    ~QTBValueGaugeRadialEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();

    void on_rangeComboBox_currentIndexChanged(int index);
    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::QTBValueGaugeRadialEditor *ui;
    QTBValueGaugeRadial *mGauge;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // HGAUGEEDITOR_H

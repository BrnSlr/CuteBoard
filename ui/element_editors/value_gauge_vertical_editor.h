#ifndef VGAUGEEDITOR_H
#define VGAUGEEDITOR_H

#include <QDialog>
#include "dashboard/elements/value_gauge_vertical.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBValueGaugeVerticalEditor;
}

class QTBValueGaugeVerticalEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QTBValueGaugeVerticalEditor(QTBValueGaugeVertical *display, QWidget *parent = nullptr);
    ~QTBValueGaugeVerticalEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();

    void on_rangeComboBox_currentIndexChanged(int index);
    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::QTBValueGaugeVerticalEditor *ui;
    QTBValueGaugeVertical *mGauge;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // VGAUGEEDITOR_H

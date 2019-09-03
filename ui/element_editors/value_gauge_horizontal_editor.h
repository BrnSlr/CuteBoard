#ifndef HGAUGEEDITOR_H
#define HGAUGEEDITOR_H

#include <QDialog>
#include "dashboard/elements/value_gauge_horizontal.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBValueGaugeHorizontalEditor;
}

class QTBValueGaugeHorizontalEditor : public QDialog
{
    Q_OBJECT

public:

    explicit QTBValueGaugeHorizontalEditor(QTBValueGaugeHorizontal *display, QWidget *parent = nullptr);
    ~QTBValueGaugeHorizontalEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();

    void on_rangeComboBox_currentIndexChanged(int index);
    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::QTBValueGaugeHorizontalEditor *ui;
    QTBValueGaugeHorizontal *mGauge;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // HGAUGEEDITOR_H

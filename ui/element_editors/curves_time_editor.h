#ifndef CURVESTIMEEDITOR_H
#define CURVESTIMEEDITOR_H

#include <QDialog>
#include "dashboard/elements/plot_time.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBCurvesTimeEditor;
}

class QTBCurvesTimeEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QTBCurvesTimeEditor(QTBPlotTime *display, QWidget *parent = nullptr);
    ~QTBCurvesTimeEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void updateTabs();
    void newParameter();
    void removeParameter();
    void on_listWidget_currentRowChanged(int currentRow);

    void on_rangeComboBox_currentIndexChanged(int index);

private:
    Ui::QTBCurvesTimeEditor *ui;
    QTBPlotTime *mDisplay;
    QList<PropertiesWidget *> mPropertiesWidgets;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // CURVESTIMEEDITOR_H

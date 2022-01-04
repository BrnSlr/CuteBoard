#ifndef CURVESXYEDITOR_H
#define CURVESXYEDITOR_H

#include <QDialog>
#include "plot_xy.h"
#include "ui/widgets/properties_widget.h"
#include "ui/board/management/curve_patron.h"
#include "plot_xy_shared.h"

namespace Ui {
class PLOT_XY_EXPORT PlotXYEditor;
}

class PLOT_XY_EXPORT PlotXYEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit PlotXYEditor(PlotXY *display, QWidget *parent = nullptr);
    ~PlotXYEditor() Q_DECL_OVERRIDE;
    void accept() Q_DECL_OVERRIDE;

    void updatePatronsTable();

private slots:
    void updateElement();
    void updateTabs();
    void newParameter();
    void removeParameter();
    void on_listWidget_currentRowChanged(int currentRow);
    void on_rangeComboBox_currentIndexChanged(int index);
    void on_xrangeComboBox_currentIndexChanged(int index);
    void on_addPatronButton_clicked();
    void on_deletePatronButton_clicked();
    void on_patronsTableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::PlotXYEditor *ui;
    PlotXY *mDisplay;
    QList<PropertiesWidget *> mPropertiesWidgets;
    PropertiesWidget * mXPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // CURVESTIMEEDITOR_H

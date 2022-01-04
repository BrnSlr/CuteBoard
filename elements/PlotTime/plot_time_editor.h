#ifndef PLOTTIMEEDITOR_H
#define PLOTTIMEEDITOR_H

#include <QDialog>
#include "plot_time.h"
#include "ui/widgets/properties_widget.h"
#include "plot_time_shared.h"

namespace Ui {
class PLOT_TIME_EXPORT PlotTimeEditor;
}

class PLOT_TIME_EXPORT PlotTimeEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit PlotTimeEditor(PlotTime *display, QWidget *parent = nullptr);
    ~PlotTimeEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void updateTabs();
    void newParameter();
    void removeParameter();
    void replaceParameter();
    void on_listWidget_currentRowChanged(int currentRow);

    void on_rangeComboBox_currentIndexChanged(int index);

private:
    Ui::PlotTimeEditor *ui;
    PlotTime *mDisplay;
    QList<PropertiesWidget *> mPropertiesWidgets;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
    QToolButton *mReplaceParamButton;
};

#endif // PLOTTIMEEDITOR_H

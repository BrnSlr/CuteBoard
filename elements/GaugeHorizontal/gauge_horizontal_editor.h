#ifndef HGAUGEEDITOR_H
#define HGAUGEEDITOR_H

#include <QDialog>
#include "gauge_horizontal.h"
#include "ui/widgets/properties_widget.h"
#include "gauge_horizontal_shared.h"

namespace Ui {
class GAUGE_HORIZONTAL_EXPORT GaugeHorizontalEditor;
}

class GAUGE_HORIZONTAL_EXPORT GaugeHorizontalEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit GaugeHorizontalEditor(GaugeHorizontal *display, QWidget *parent = nullptr);
    ~GaugeHorizontalEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();
    void replaceParameter();

    void on_rangeComboBox_currentIndexChanged(int index);
    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::GaugeHorizontalEditor *ui;
    GaugeHorizontal *mGauge;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
    QToolButton *mReplaceParamButton;
};

#endif // HGAUGEEDITOR_H

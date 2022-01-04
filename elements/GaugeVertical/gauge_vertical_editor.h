#ifndef VGAUGEEDITOR_H
#define VGAUGEEDITOR_H

#include <QDialog>
#include "gauge_vertical.h"
#include "gauge_vertical_shared.h"
#include "ui/widgets/properties_widget.h"

namespace Ui {
class GAUGE_VERTICAL_EXPORT GaugeVerticalEditor;
}

class GAUGE_VERTICAL_EXPORT GaugeVerticalEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit GaugeVerticalEditor(GaugeVertical *display, QWidget *parent = nullptr);
    ~GaugeVerticalEditor() Q_DECL_OVERRIDE;

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
    Ui::GaugeVerticalEditor *ui;
    GaugeVertical *mGauge;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
    QToolButton *mReplaceParamButton;
};

#endif // VGAUGEEDITOR_H

#ifndef CGAUGEEDITOR_H
#define CGAUGEEDITOR_H

#include <QDialog>
#include "gauge_radial.h"
#include "ui/widgets/properties_widget.h"
#include "gauge_radial_shared.h"

namespace Ui {
class GAUGE_RADIAL_EXPORT GaugeRadialEditor;
}

class GAUGE_RADIAL_EXPORT GaugeRadialEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:

    explicit GaugeRadialEditor(GaugeRadial *display, QWidget *parent = nullptr);
    ~GaugeRadialEditor() Q_DECL_OVERRIDE;

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
    Ui::GaugeRadialEditor *ui;
    GaugeRadial *mGauge;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
    QToolButton *mReplaceParamButton;
};

#endif // HGAUGEEDITOR_H

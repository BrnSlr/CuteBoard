#ifndef VALUEDISPLAYEDITOR_H
#define VALUEDISPLAYEDITOR_H

#include <QDialog>
#include "dashboard/elements/value_display.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBValueDisplayEditor;
}

class QTBValueDisplayEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QTBValueDisplayEditor(QTBValueDisplay *display, QWidget *parent = nullptr);
    ~QTBValueDisplayEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();

private:
    Ui::QTBValueDisplayEditor *ui;
    QTBValueDisplay *mValueDisplay;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // VALUEDISPLAYEDITOR_H

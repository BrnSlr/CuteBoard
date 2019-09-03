#ifndef STATEINDICATOREDITOR_H
#define STATEINDICATOREDITOR_H

#include <QDialog>
#include "dashboard/elements/state_display.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBStateDisplayEditor;
}

class QTBStateDisplayEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QTBStateDisplayEditor(QTBStateDisplay *display, QWidget *parent = nullptr);
    ~QTBStateDisplayEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();

private:
    Ui::QTBStateDisplayEditor *ui;
    QTBStateDisplay *mStateIndicator;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // STATEINDICATOREDITOR_H

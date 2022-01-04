#ifndef NUMERICALDISPLAYEDITOR_H
#define NUMERICALDISPLAYEDITOR_H

#include <QDialog>
#include "numerical_display.h"
#include "ui/widgets/properties_widget.h"
#include "numerical_display_shared.h"

namespace Ui {
class NUMERICAL_DISPLAY_EXPORT NumericalDisplayEditor;
}

class NUMERICAL_DISPLAY_EXPORT NumericalDisplayEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit NumericalDisplayEditor(NumericalDisplay *display, QWidget *parent = nullptr);
    ~NumericalDisplayEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();
    void replaceParameter();

private:
    Ui::NumericalDisplayEditor *ui;
    NumericalDisplay *mValueDisplay;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
    QToolButton *mReplaceParamButton;
};

#endif // VALUEDISPLAYEDITOR_H

#ifndef STATEDISPLAYEDITOR_H
#define STATEDISPLAYEDITOR_H

#include <QDialog>
#include "state_display.h"
#include "ui/widgets/properties_widget.h"
#include "state_display_shared.h"

namespace Ui {
class STATE_DISPLAY_EXPORT StateDisplayEditor;
}

class STATE_DISPLAY_EXPORT StateDisplayEditor : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit StateDisplayEditor(StateDisplay *display, QWidget *parent = nullptr);
    ~StateDisplayEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();
    void replaceParameter();

private:
    Ui::StateDisplayEditor *ui;
    StateDisplay *mStateIndicator;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
    QToolButton *mReplaceParamButton;
};

#endif // STATEDISPLAYEDITOR_H

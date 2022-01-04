#ifndef STATE_TEXT_H
#define STATE_TEXT_H

#include "../NumericalDisplay/numerical_display.h"
#include "state_display_shared.h"

class STATE_DISPLAY_EXPORT StateDisplay: public NumericalDisplay
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    StateDisplay(Board *dashboard = nullptr);

    virtual void edit() Q_DECL_OVERRIDE;

    virtual void initialize(Board *dashboard) Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;

    void displayData() Q_DECL_OVERRIDE;

private:
    QPen mTextBorderPen;
};

#endif // STATE_TEXT_H

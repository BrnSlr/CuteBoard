#ifndef STATE_TEXT_H
#define STATE_TEXT_H

#include "value_display.h"

#define STATETEXT_NAME "State - Display"

class QTBStateDisplay: public QTBValueDisplay
{
    Q_OBJECT
public:
    QTBStateDisplay(QTBoard *dashboard = nullptr);

    virtual void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(STATETEXT_NAME);}

    virtual void updateDashboardParameters(QTBDashboardParameter::UpdateMode mode = QTBDashboardParameter::umValue) Q_DECL_OVERRIDE;
    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;

    void processNewSamples() Q_DECL_OVERRIDE;
};

static ElementRegister<QTBStateDisplay> stateTextRegister(QString(STATETEXT_NAME), QTBDashboardElement::etSingleParam,":/elements/icons8_color_50px.png");

#endif // STATE_TEXT_H

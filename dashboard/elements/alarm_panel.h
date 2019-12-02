#ifndef ALARM_PANEL_H
#define ALARM_PANEL_H

#include "dashboard/dashboard_element.h"
#include "dashboard/elements_base/adjust_text_element.h"
#include "dashboard/layouts/layout_list.h"
#include "dashboard/elements_factory/elementfactory.h"
#include "project/alarm_configuration.h"

#define ALARMPANEL_NAME "Alarm Panel"

class QTBAlarmPanel : public QTBDashboardElement
{
    Q_OBJECT
public:
    QTBAlarmPanel(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(ALARMPANEL_NAME);}

    void updateElement() Q_DECL_OVERRIDE;
    void update(UpdatePhase phase) Q_DECL_OVERRIDE;

    void updateDashboardParameters(QTBDashboardParameter::UpdateMode mode = QTBDashboardParameter::umValue) Q_DECL_OVERRIDE;
    void processNewSamples() Q_DECL_OVERRIDE;
    void checkParameters() Q_DECL_OVERRIDE;

    void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;
    void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;

    void saveConfigurations(QSettings *settings) Q_DECL_OVERRIDE;
    void loadConfigurations(QSettings *settings) Q_DECL_OVERRIDE;

    QSharedPointer<QTBDashboardParameter> addParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings) Q_DECL_OVERRIDE;
    QSharedPointer<QTBDashboardParameter> addParameter(QString paramLabel) Q_DECL_OVERRIDE;
    QSharedPointer<QTBDashboardParameter> addParameter(QSharedPointer<QTBParameter> dataParameter) Q_DECL_OVERRIDE;

    void addAlarm(QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig);
    void updateAlarmConfiguration();

    bool connected() const;
    void disconnectAlarmConfig();

    void beforeReplot() Q_DECL_OVERRIDE;
    void afterReplot() Q_DECL_OVERRIDE;

protected:
    QTBAdjustTextElement *mHeader;
    QList<QTBAdjustTextElement *> mTextElements;
    QTBLayoutList *mMainLayout;

    QList<QSharedPointer<QTBDashboardParameter>> mDashParametersPrimary;
    QList<QSharedPointer<QTBDashboardParameter>> mDashParametersSecondary;

    bool mAlarmConfigurationIsShared;
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> mSharedAlarmConfiguration;
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> mExclusiveAlarmConfiguration;
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> mAlarmConfiguration;

};

static ElementRegister<QTBAlarmPanel> alarmRegister(QString(ALARMPANEL_NAME), QTBDashboardElement::etAlarm,":/elements/icons8_alarms_50px.png");

#endif // ALARM_PANEL_H

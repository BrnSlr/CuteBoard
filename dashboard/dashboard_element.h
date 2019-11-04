#ifndef DASHBOARD_ELEMENT_H
#define DASHBOARD_ELEMENT_H

#include "dashboard/layouts/layout_reactive_element.h"
#include "dashboard/dashboard_parameter.h"

class QTBDashboardElement : public QTBLayoutReactiveElement
{
    Q_OBJECT
public:

    enum ElementType {
        etUnknown,
        etAlarm,
        etSingleParam,
        etMultiParam,
        etOther
    };

    QTBDashboardElement(QTBoard *dashboard = nullptr);

    virtual void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    virtual void edit() {}
    virtual QString name() {return QString();}

    virtual void loadSettings(QSettings *settings) {Q_UNUSED(settings)}
    virtual void saveSettings(QSettings *settings){Q_UNUSED(settings)}

    virtual void droppedParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);
    virtual void droppedParameter(QSharedPointer<QTBParameter> dataParameter);

    virtual QSharedPointer<QTBDashboardParameter> addParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);
    virtual QSharedPointer<QTBDashboardParameter> addParameter(QString paramLabel);
    virtual QSharedPointer<QTBDashboardParameter> addParameter(QSharedPointer<QTBParameter> dataParameter);
    virtual void addParameter(QSharedPointer<QTBDashboardParameter> dashParameter);

    virtual void updateDashboardParameters(QTBDashboardParameter::UpdateMode mode = QTBDashboardParameter::umValue);
    virtual void processNewSamples() {}
    virtual void processHistoricalSamples() {}
    virtual void updateElement() {}
    virtual void checkParameters();

    int parametersMaxCount() const;
    void setParametersMaxCount(int parametersMaxCount);

    virtual void saveConfigurations(QSettings *settings);
    virtual void loadConfigurations(QSettings *settings);

    int parametersCount() {return mDashParameters.count();}
    QSharedPointer<QTBDashboardParameter> dashParameter(int index);
    virtual void removeDashParameter(int index);
    void removeAllDashParameter();

    QTBParameterConfiguration::ConfigurationMode configurationMode() const;
    void setConfigurationMode(const QTBParameterConfiguration::ConfigurationMode &configurationMode);

    ElementType type() const;
    void setType(const ElementType &type);

public slots:
    virtual void beforeReplot();
    virtual void afterReplot();

protected:
    QTBoard *mBoard;
    int mParametersMaxCount;
    ElementType mType;
    QList<QSharedPointer<QTBDashboardParameter>> mDashParameters;
    QList<QString> mParametersLabel;
    QTBParameterConfiguration::ConfigurationMode mConfigurationMode;

};

#endif // DASHBOARD_ELEMENT_H

#ifndef DASHBOARD_ELEMENT_H
#define DASHBOARD_ELEMENT_H

#include "dashboard/layouts/layout_reactive_element.h"
#include "dashboard/dashboard_parameter.h"

class QTBDashboardElement : public QTBLayoutReactiveElement
{
    Q_OBJECT
public:
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
    virtual void addDashboardParameter(QSharedPointer<QTBDashboardParameter> dashParameter);

    virtual void updateDashboardParameters(QTBDashboardParameter::UpdateMode mode = QTBDashboardParameter::umValue);
    virtual void processNewSamples() {}
    virtual void processHistoricalSamples() {}
    virtual void updateElement() {}
    virtual void checkParameters();

    int parametersMaxCount() const;
    void setParametersMaxCount(int parametersMaxCount);

    virtual void saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull);
    virtual void loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull);

    int parametersCount() {return mDashParameters.count();}
    QSharedPointer<QTBDashboardParameter> dashParameter(int index);
    virtual void removeDashParameter(int index);
    void removeAllDashParameter();

public slots:
    virtual void beforeReplot();
    virtual void afterReplot();

protected:
    QTBoard *mBoard;
    int mParametersMaxCount;
    QList<QSharedPointer<QTBDashboardParameter>> mDashParameters;

};

#endif // DASHBOARD_ELEMENT_H

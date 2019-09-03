#ifndef VISUALISATIONPARAMETER_H
#define VISUALISATIONPARAMETER_H

#include <QColor>
#include <QBrush>
#include <QSettings>
#include "data/data_parameter.h"
#include "project/parameter_configuration.h"
#include "data/data_sample.h"

enum ValueStringFormat {
    vsfDecimal,
    vsfHexa,
    vsfHexaLsb16,
    vsfHexaMsb16
};

class QTBoard;
class QTBDashboardParameter
{
public:
    enum UpdateMode {
        umValueOnly,
        umValue,
        umState
    };

    QTBDashboardParameter(QTBoard *board);
    QTBDashboardParameter(const QSharedPointer<QTBParameter>& dataParameter, QTBoard *board);
    QTBDashboardParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> paramProperties, QTBoard *board);
    QTBDashboardParameter(const QString& parameterLabel, QTBoard *board);
    ~QTBDashboardParameter();

    QString getDisplayedLabel() const;
    QString getDisplayedUnit() const;

    QString getValueString(ValueStringFormat format = vsfDecimal) const;
    double getValueDouble() const;
    quint32 getValueBinaryWeight32() const;
    QString getStateString() const;
    QString getBitDescription(int bitNumber) const;
    bool getBitLogic(int bitNumber) const;

    void update(UpdateMode mode = umValue);

    bool configurationChanged();
    void propertiesChecked();

    bool connected() const;
    void disconnectProperties();

    void saveParameterSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull);
    void loadParameterSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull);

    QExplicitlySharedDataPointer<QTBParameterConfiguration> sharedParameterConfiguration() const;
    QExplicitlySharedDataPointer<QTBParameterConfiguration> exclusiveParameterConfiguration() const;
    QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterConfiguration() const;

    double getTimestamp() const;

    QColor getColor() const;
    QColor getForegroundColor() const;
    QBrush getBackgroundBrush() const;

    QString getLabel() const;
    void setLabel(const QString &label);

    QString getUnit() const;
    void setUnit(const QString &unit);

    quint32 getParameterId() const;
    void setParameterId(const quint32 &parameterId);

protected:
    quint32 mParameterId;
    QString mLabel;
    QString mUnit;

    QTBDataSample mSample;

    QColor mColor;
    QColor mForegroundColor;
    QBrush mBackgroundBrush;

    QTBoard *mBoard;

    bool mParameterConfigurationIsShared;
    QExplicitlySharedDataPointer<QTBParameterConfiguration> mSharedParameterConfiguration;
    QExplicitlySharedDataPointer<QTBParameterConfiguration> mExclusiveParameterConfiguration;
    QExplicitlySharedDataPointer<QTBParameterConfiguration> mParameterConfiguration;

};

#endif // VISUALISATIONPARAMETER_H

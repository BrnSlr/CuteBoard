#ifndef VISUALISATIONPARAMETER_H
#define VISUALISATIONPARAMETER_H

#include <QColor>
#include <QBrush>
#include <QSettings>
#include "data/time_series.h"
#include "ui/board/management/parameter_configuration.h"
#include "ui/ui_global.h"

enum ValueStringFormat {
    vsfDecimal,
    vsfHexa,
    vsfHexaLsb16,
    vsfHexaMsb16,
    vsfTimeFromSec,
    vsfBinary
};

class Board;
class BOARD_UI_EXPORT BoardParameter
{
public:
    BoardParameter(Board *board);
    BoardParameter(const QSharedPointer<TimeSeries>& dataParameter, Board *board);
    BoardParameter(QExplicitlySharedDataPointer<ParameterConfiguration> paramProperties, Board *board);
    BoardParameter(const QString& parameterLabel, Board *board);

    QString getDisplayedLabel() const;
    QString getDisplayedUnit() const;

    QString getValueString(ValueStringFormat format = vsfDecimal) const;
    double getValueDouble() const;
    quint32 getValueBinaryWeight32() const;
    QString getStateString() const;
    QString getBitDescription(int bitNumber) const;
    bool getBitLogic(int bitNumber) const;

    void updateData();
    void processValueData();
    void processValueData(double value);
    void processStateData();

    bool configurationHasChanged();
    void modificationsApplied();

    bool connected() const;
    void disconnectSharedConfiguration(bool resetSharedConfig = false);

    void saveParameterSettings(QSettings *settings, ParameterConfiguration::ConfigurationMode mode = ParameterConfiguration::cmFull);
    void loadParameterSettings(QSettings *settings, ParameterConfiguration::ConfigurationMode mode = ParameterConfiguration::cmFull);

    QExplicitlySharedDataPointer<ParameterConfiguration> sharedParameterConfiguration() const;
    QExplicitlySharedDataPointer<ParameterConfiguration> exclusiveParameterConfiguration() const;
    QExplicitlySharedDataPointer<ParameterConfiguration> parameterConfiguration() const;

    void setSharedParameterConfiguration(const QExplicitlySharedDataPointer<ParameterConfiguration> &sharedParameterConfiguration, bool connected);
    void setExclusiveParameterConfiguration(const QExplicitlySharedDataPointer<ParameterConfiguration> &exclusiveParameterConfiguration);

    double getTimestamp() const;

    QColor getColor() const;
    QColor getForegroundColor() const;
    QBrush getBackgroundBrush() const;

    TimeSeriesParameter getTimeSeries() const;
    void setTimeSeries(const TimeSeriesParameter &timeSeries);


protected:
    double mTimestamp;
    DataValue mValue;

    QColor mColor;
    QColor mForegroundColor;
    QBrush mBackgroundBrush;

    Board *mBoard;

    TimeSeriesParameter mTimeSeries;

    bool mParameterConfigurationIsShared;
    QExplicitlySharedDataPointer<ParameterConfiguration> mSharedParameterConfiguration;
    QExplicitlySharedDataPointer<ParameterConfiguration> mExclusiveParameterConfiguration;
    QExplicitlySharedDataPointer<ParameterConfiguration> mParameterConfiguration;
};

#endif // VISUALISATIONPARAMETER_H

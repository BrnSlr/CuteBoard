#include <QDateTime>
#include <qmath.h>
#include "board.h"
#include "ui/board/management/project.h"
#include "board_parameter.h"

BoardParameter::BoardParameter(Board *board):
    mBoard(board),
    mTimeSeries(),
    mParameterConfigurationIsShared(false),
    mSharedParameterConfiguration(nullptr),
    mExclusiveParameterConfiguration(nullptr),
    mParameterConfiguration(new ParameterConfiguration ())
{
}

BoardParameter::BoardParameter(const QSharedPointer<TimeSeries>& dataParameter, Board *board):
    mBoard(board),
    mTimeSeries(*dataParameter.data()),
    mParameterConfigurationIsShared(false),
    mSharedParameterConfiguration(nullptr),
    mExclusiveParameterConfiguration(new ParameterConfiguration ()),
    mParameterConfiguration(new ParameterConfiguration ())
{
    mExclusiveParameterConfiguration->setLabel(mTimeSeries.name());
    mParameterConfiguration = mExclusiveParameterConfiguration;
    mParameterConfiguration->defaultColorSettingsRef().setColor(mBoard->randomColor());
}

BoardParameter::BoardParameter(QExplicitlySharedDataPointer<ParameterConfiguration> paramProperties, Board *board):
    mBoard(board),
    mTimeSeries(),
    mParameterConfigurationIsShared(true),
    mSharedParameterConfiguration(paramProperties),
    mExclusiveParameterConfiguration(new ParameterConfiguration ()),
    mParameterConfiguration(new ParameterConfiguration ())
{
    if(paramProperties) {
        mTimeSeries.setName(paramProperties->label());
        mExclusiveParameterConfiguration = paramProperties;
        mExclusiveParameterConfiguration.detach();
        mParameterConfiguration = mSharedParameterConfiguration;
    }
}

BoardParameter::BoardParameter(const QString& parameterLabel, Board *board) :
    mBoard(board),
    mTimeSeries(),
    mParameterConfigurationIsShared(false),
    mSharedParameterConfiguration(nullptr),
    mExclusiveParameterConfiguration(new ParameterConfiguration ()),
    mParameterConfiguration(new ParameterConfiguration ())
{
    if(!parameterLabel.isEmpty()) {
        mTimeSeries.setName(parameterLabel);
        mExclusiveParameterConfiguration->setLabel(mTimeSeries.name());
        mParameterConfiguration = mExclusiveParameterConfiguration;
        mParameterConfiguration->defaultColorSettingsRef().setColor(mBoard->randomColor());
    }
}

QString BoardParameter::getDisplayedLabel() const
{
    if(mParameterConfiguration->userLabelEnabled()) {
        return mParameterConfiguration->userDefinedLabel();
    }
    return mParameterConfiguration->label();
}

QString BoardParameter::getDisplayedUnit() const
{
    if(mParameterConfiguration->userUnitEnabled()) {
        return mParameterConfiguration->userDefinedUnit();
    }
    return mTimeSeries.unit();
}

QString BoardParameter::getValueString(ValueStringFormat format) const
{
    if(mTimeSeries.parameterId() > 0) {
        switch(format) {
        case vsfDecimal:
            return mValue.toString(mParameterConfiguration->precision());
        case vsfHexa:
        {
            return mValue.toHex();
        }
        case vsfHexaLsb16:
        {
            struct s_fToBits
            {
                quint16     lsb;
                quint16     msb;
            };

            union int32_int16{
                s_fToBits fToBits;
                quint32 ui32;
            } toUint32{};

            toUint32.ui32 = mValue.uint32_value();
            quint32 value =  toUint32.fToBits.lsb;
            return QString("0x")+QString("%1").arg(value , 4, 16, QChar('0')).toUpper();
        }
        case vsfHexaMsb16:
        {
            struct s_fToBits
            {
                quint16     lsb;
                quint16     msb;
            };

            union int32_int16{
                s_fToBits fToBits;
                quint32 ui32;
            } toUint32{};

            toUint32.ui32 = mValue.uint32_value();
            quint32 value =  toUint32.fToBits.msb;
            return QString("0x")+QString("%1").arg(value , 4, 16, QChar('0')).toUpper();
        }
        case vsfTimeFromSec:
            return mValue.toTime();
        case vsfBinary:
            return mValue.toBin();
        }
    }
    return QString("X");
}

double BoardParameter::getValueDouble() const
{
    if(mTimeSeries.parameterId() > 0) {
        return mValue.toDouble();
    }
    return qQNaN();
}

quint32 BoardParameter::getValueBinaryWeight32() const
{
    if(mTimeSeries.parameterId() > 0) {
        return mValue.uint32_value();
    }
    return 0;
}

QString BoardParameter::getStateString() const
{
    QString text;
    if(mTimeSeries.parameterId() > 0) {
        if(mParameterConfiguration->statesSettingsRef().active()) {
            text = mParameterConfiguration->statesSettingsRef().text(mValue.toUint32());
        }
    }
    return text;
}

QString BoardParameter::getBitDescription(int bitNumber) const
{
    return mParameterConfiguration->bitfieldsSettingsRef().bitDescriptions().at(bitNumber);
}

bool BoardParameter::getBitLogic(int bitNumber) const
{
    return mParameterConfiguration->bitfieldsSettingsRef().bitLogics().at(bitNumber);
}

void BoardParameter::updateData()
{
    if(mTimeSeries.parameterId() > 0) {
        mBoard->dataManager()->lastBoardData(mTimeSeries.parameterId(), mValue, mTimestamp);
    }
}

void BoardParameter::processValueData()
{
    mColor = mParameterConfiguration->defaultColorSettingsRef().color();
    mForegroundColor = mParameterConfiguration->defaultColorSettingsRef().foregroundColor();
    mBackgroundBrush = mParameterConfiguration->defaultColorSettingsRef().backgroundBrush();

    if(mTimeSeries.parameterId() > 0) {
        bool validColor = false;
        double value = mValue.toDouble();
        if(mParameterConfiguration->validRange() && mParameterConfiguration->outOfRangeColorEnabled()) {
            if(value < mParameterConfiguration->rangeMinimum() || value > mParameterConfiguration->rangeMaximum() ) {
                mColor = mParameterConfiguration->outOfRangeColorSettingsRef().color();
                mForegroundColor = mParameterConfiguration->outOfRangeColorSettingsRef().foregroundColor();
                mBackgroundBrush = mParameterConfiguration->outOfRangeColorSettingsRef().backgroundBrush();
                validColor = true;
            }
        }
        if(!validColor) {
            if(mParameterConfiguration->thresholdsSettingsRef().active()) {
                ColorSettings cs = mParameterConfiguration->thresholdsSettingsRef().colorSettings(value,validColor);
                if(validColor) {
                    mColor = cs.color();
                    mForegroundColor = cs.foregroundColor();
                    mBackgroundBrush = cs.backgroundBrush();
                }
            }
        }
    }
}

void BoardParameter::processValueData(double value)
{
    if(mTimeSeries.parameterId() > 0) {
        bool validColor = false;
        if(mParameterConfiguration->validRange() && mParameterConfiguration->outOfRangeColorEnabled()) {
            if(value < mParameterConfiguration->rangeMinimum() || value > mParameterConfiguration->rangeMaximum() ) {
                mColor = mParameterConfiguration->outOfRangeColorSettingsRef().color();
                mForegroundColor = mParameterConfiguration->outOfRangeColorSettingsRef().foregroundColor();
                mBackgroundBrush = mParameterConfiguration->outOfRangeColorSettingsRef().backgroundBrush();
                validColor = true;
            }
        }
        if(!validColor) {
            if(mParameterConfiguration->thresholdsSettingsRef().active()) {
                ColorSettings cs = mParameterConfiguration->thresholdsSettingsRef().colorSettings(value,validColor);
                if(validColor) {
                    mColor = cs.color();
                    mForegroundColor = cs.foregroundColor();
                    mBackgroundBrush = cs.backgroundBrush();
                }
            }
        }
    }
}

void BoardParameter::processStateData()
{
    mColor = mParameterConfiguration->defaultColorSettingsRef().color();
    mForegroundColor = mParameterConfiguration->defaultColorSettingsRef().foregroundColor();
    mBackgroundBrush = Qt::NoBrush;

    if(mTimeSeries.parameterId() > 0) {
        bool validColor = false;
        if(mParameterConfiguration->statesSettingsRef().active()) {
            ColorSettings cs = mParameterConfiguration->statesSettingsRef().colorSettings( mValue.toUint32(),validColor);
            if(validColor) {
                mColor = cs.color();
                mForegroundColor = cs.foregroundColor();
                mBackgroundBrush = cs.backgroundBrush();
            }
        }
    }
}

bool BoardParameter::configurationHasChanged()
{
    return mParameterConfiguration->modified();
}

void BoardParameter::modificationsApplied()
{
    mParameterConfiguration->setModified(false);
}

bool BoardParameter::connected() const
{
    return mParameterConfigurationIsShared;
}

void BoardParameter::disconnectSharedConfiguration(bool resetSharedConfig)
{
    mParameterConfigurationIsShared = false;
    if(resetSharedConfig)
        mSharedParameterConfiguration.reset();
    mParameterConfiguration = mExclusiveParameterConfiguration;
}

void BoardParameter::saveParameterSettings(QSettings *settings, ParameterConfiguration::ConfigurationMode mode)
{
    settings->setValue("Connected", mParameterConfigurationIsShared);
    if(mParameterConfigurationIsShared) {
        settings->setValue("ConnectedParamLabel", mSharedParameterConfiguration->label());
        settings->setValue("ConnectedProperties", mSharedParameterConfiguration->description());
    } else {
        mExclusiveParameterConfiguration->save(settings, mode);
    }
}

void BoardParameter::loadParameterSettings(QSettings *settings, ParameterConfiguration::ConfigurationMode mode)
{
    mParameterConfigurationIsShared = settings->value("Connected").toBool();
    if(mParameterConfigurationIsShared) {
        QString paramlabel = settings->value("ConnectedParamLabel").toString();
        QString properties = settings->value("ConnectedProperties").toString();
        QExplicitlySharedDataPointer<ParameterConfiguration> prop = mBoard->project()->parameterSettings(paramlabel, properties);
        if(prop) {
            mSharedParameterConfiguration = prop;
            mTimeSeries.setName( mSharedParameterConfiguration->label());
            mExclusiveParameterConfiguration = prop;
            mExclusiveParameterConfiguration.detach();
            mParameterConfigurationIsShared = true;
            mParameterConfiguration = mSharedParameterConfiguration;
        } else {
            mParameterConfigurationIsShared = false;
            mTimeSeries.setName(paramlabel);
            if(!mExclusiveParameterConfiguration) {
                mExclusiveParameterConfiguration = QExplicitlySharedDataPointer<ParameterConfiguration>(new ParameterConfiguration());
            }
            mExclusiveParameterConfiguration->setLabel(mTimeSeries.name());
            mParameterConfiguration = mExclusiveParameterConfiguration;
            mParameterConfiguration->defaultColorSettingsRef().setColor(mBoard->randomColor());
        }
    } else {
        if(!mExclusiveParameterConfiguration) {
            mExclusiveParameterConfiguration = QExplicitlySharedDataPointer<ParameterConfiguration>(new ParameterConfiguration());
        }
        mExclusiveParameterConfiguration->load(settings, mode);
        mTimeSeries.setName(mExclusiveParameterConfiguration->label());
        mParameterConfigurationIsShared = false;
        mParameterConfiguration = mExclusiveParameterConfiguration;
    }
}

QExplicitlySharedDataPointer<ParameterConfiguration> BoardParameter::sharedParameterConfiguration() const
{
    return mSharedParameterConfiguration;
}

QExplicitlySharedDataPointer<ParameterConfiguration> BoardParameter::exclusiveParameterConfiguration() const
{
    return mExclusiveParameterConfiguration;
}

double BoardParameter::getTimestamp() const
{
    if(mTimeSeries.parameterId() > 0) {
        return mTimestamp;
    }
    return 0;
}

QColor BoardParameter::getColor() const
{
    return mColor;
}

QColor BoardParameter::getForegroundColor() const
{
    return mForegroundColor;
}

QBrush BoardParameter::getBackgroundBrush() const
{
    return mBackgroundBrush;
}

TimeSeriesParameter BoardParameter::getTimeSeries() const
{
    return mTimeSeries;
}

void BoardParameter::setTimeSeries(const TimeSeriesParameter &timeSeries)
{
    mTimeSeries = timeSeries;
}

void BoardParameter::setExclusiveParameterConfiguration(const QExplicitlySharedDataPointer<ParameterConfiguration> &exclusiveParameterConfiguration)
{
    mExclusiveParameterConfiguration = exclusiveParameterConfiguration;
    mParameterConfiguration = mExclusiveParameterConfiguration;
}

void BoardParameter::setSharedParameterConfiguration(const QExplicitlySharedDataPointer<ParameterConfiguration> &sharedParameterConfiguration, bool connected)
{
    mSharedParameterConfiguration = sharedParameterConfiguration;
    if(connected) {
        mParameterConfigurationIsShared = true;
        mParameterConfiguration = mSharedParameterConfiguration;
    }
}

QExplicitlySharedDataPointer<ParameterConfiguration> BoardParameter::parameterConfiguration() const
{
    return mParameterConfiguration;
}

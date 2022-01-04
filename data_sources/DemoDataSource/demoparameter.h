#ifndef DEMOPARAMETER_H
#define DEMOPARAMETER_H

#include <QObject>
#include <QtMath>
#include <QRandomGenerator>
#include "time_series.h"
#include "data.h"
#include "log4qt/logger.h"

template <class InType, class OutType>
void safeTypeConversion(const InType &inVal, OutType &outVal)
{
    if(std::is_same<InType, OutType>::value) {
        outVal = inVal;
    } else {
        if( inVal > std::numeric_limits<OutType>::max()) {
            outVal = std::numeric_limits<OutType>::max();
        } else if(inVal < std::numeric_limits<OutType>::lowest()) {
            outVal = std::numeric_limits<OutType>::lowest();
        } else {
            outVal = static_cast<OutType>(inVal);
        }
    }
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class DemoParameter: public QObject, public TimeSeries
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum GeneratorType
    {
        gtSine,
        gtSquare,
        gtTriangle,
        gtSawtooth,
        gtCustom,
    };

    DemoParameter(QString label,
                  int nbSamples = 96000,
                  int sample_size = 1,
                  QObject *parent = nullptr);

    DataValue updateParameter(double timeSec);
    void updateParameterVec(double timeSec);

    template <class T> void updateValue(double timeSec, T &outValue)
    {
        double value = 0.0;
        double t = mSignalFrequency * timeSec + mSignalPhase;

        switch (mGeneratorType)
        { // http://en.wikipedia.org/wiki/Waveform
        case GeneratorType::gtSine :
            value = qSin(2.0*M_PI*t);
            break;
        case GeneratorType::gtSquare:
            value = sgn(qSin(2.0*M_PI*t));
            break;
        case GeneratorType::gtTriangle:
            value = 1.0-4.0*qAbs( qRound(t-0.25)-(t-0.25) );
            break;
        case GeneratorType::gtSawtooth:
            value = 2.0*(t-qFloor(t+0.5));
            break;
        case GeneratorType::gtCustom :
            value = qSin(2.0*M_PI*t)+qrand()/double(RAND_MAX)*1*qSin(2.0*M_PI*t/0.3843);
            break;
        }

        value = mSignalAmplitude*value+mSignalOffset;
        safeTypeConversion(value, outValue);
    }

    GeneratorType generatorType() const;
    DataValue::ValueType valueType() const;
    double signalFrequency() const;
    double signalPhase() const;
    double signalAmplitude() const;
    double signalOffset() const;

    void setGeneratorType(const GeneratorType &generatorType);
    void setValueType(const DataValue::ValueType &valueType);
    void setSignalFrequency(double signalFrequency);
    void setSignalPhase(double signalPhase);
    void setSignalAmplitude(double signalAmplitude);
    void setSignalOffset(double signalOffset);

    int refreshPeriod() const;
    void setRefreshPeriod(int refreshPeriod);

    int samplesPerPeriod() const;
    void setSamplesPerPeriod(int samplesPerPeriod);

protected:

    GeneratorType mGeneratorType;
    DataValue::ValueType mValueType;

    double mSignalFrequency;
    double mSignalPhase;
    double mSignalAmplitude;
    double mSignalOffset;
    int mRefreshPeriod;
    int mSamplesPerPeriod;
};

#endif // DEMOPARAMETER_H

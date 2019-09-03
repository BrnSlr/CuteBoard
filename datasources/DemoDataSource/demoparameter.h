#ifndef DEMOPARAMETER_H
#define DEMOPARAMETER_H

#include <QObject>
#include <QtMath>
#include "data/data_parameter.h"
#include "data/data_value.h"

template <class InType, class OutType>
void safeTypeConversion(const InType &inVal, OutType &outVal)
{
    if(std::is_same<InType, OutType>::value)
    {
        outVal = inVal;
    }
    else
    {
        if( inVal > std::numeric_limits<OutType>::max())
        {
            outVal = std::numeric_limits<OutType>::max();
        }
        else if(inVal < std::numeric_limits<OutType>::min())
        {
            outVal = std::numeric_limits<OutType>::min();
        }
        else
        {
            outVal = static_cast<OutType>(inVal);
        }
    }
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class DemoParameter: public QObject, public QTBParameter
{
    Q_OBJECT
public:
    enum GeneratorType
    {
        gtSine,
        gtSquare,
        gtTriangle,
        gtSawtooth
    };

    DemoParameter(QString label, QObject *parent = nullptr);

    QTBDataValue updateParameter(double timeSec);

    template <class T> void updateValue(double timeSec, T &outValue)
    {
        float value = 0.0;
        double t = mSignalFrequency * timeSec + mSignalPhase;

        switch (mGeneratorType)
        { // http://en.wikipedia.org/wiki/Waveform
        case GeneratorType::gtSine : // sin( 2 * pi * t )
            value = float(qSin(2.0*M_PI*t));
            break;
        case GeneratorType::gtSquare: // sign( sin( 2 * pi * t ) )
            value = sgn(qSin(2.0*M_PI*t));
            break;
        case GeneratorType::gtTriangle: // 2 * abs( t - 2 * floor( t / 2 ) - 1 ) - 1
            value = float(1.0-4.0*qAbs( qRound(t-0.25)-(t-0.25) ));
            break;
        case GeneratorType::gtSawtooth:  // 2 * ( t/a - floor( t/a + 1/2 ) )
            value = float(2.0*(t-qFloor(t+0.5)));
            break;
        }

        value = mSignalAmplitude*value+mSignalOffset;
        safeTypeConversion(value, outValue);
    }

    GeneratorType generatorType() const;
    QTBDataValue::ValueType valueType() const;
    double signalFrequency() const;
    double signalPhase() const;
    float signalAmplitude() const;
    float signalOffset() const;

    void setGeneratorType(const GeneratorType &generatorType);
    void setValueType(const QTBDataValue::ValueType &valueType);
    void setSignalFrequency(double signalFrequency);
    void setSignalPhase(double signalPhase);
    void setSignalAmplitude(float signalAmplitude);
    void setSignalOffset(float signalOffset);

protected:

    GeneratorType mGeneratorType;
    QTBDataValue::ValueType mValueType;

    double mSignalFrequency;
    double mSignalPhase;
    float mSignalAmplitude;
    float mSignalOffset;
};

#endif // DEMOPARAMETER_H

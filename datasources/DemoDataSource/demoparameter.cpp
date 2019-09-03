#include "demoparameter.h"

DemoParameter::DemoParameter(QString label, QObject *parent) :
    QObject(parent),
    QTBParameter(),
    mGeneratorType(gtSine),
    mValueType(QTBDataValue::TYPE_FLOAT),
    mSignalFrequency(0.1),
    mSignalPhase(0),
    mSignalAmplitude(100),
    mSignalOffset(1000)
{
    setLabel(label);
}

DemoParameter::GeneratorType DemoParameter::generatorType() const
{
    return mGeneratorType;
}

void DemoParameter::setGeneratorType(const GeneratorType &generatorType)
{
    mGeneratorType = generatorType;
}

QTBDataValue::ValueType DemoParameter::valueType() const
{
    return mValueType;
}

void DemoParameter::setValueType(const QTBDataValue::ValueType &valueType)
{
    mValueType = valueType;
}

QTBDataValue DemoParameter::updateParameter(double timeSec)
{
    switch(mValueType) {
    case QTBDataValue::TYPE_INT8:
    {
        int8_t      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    case QTBDataValue::TYPE_UINT8:
    {
        uint8_t      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    case QTBDataValue::TYPE_INT16:
    {
        int16_t      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    case QTBDataValue::TYPE_UINT16:
    {
        uint16_t      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    case QTBDataValue::TYPE_INT32:
    {
        int32_t      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    case QTBDataValue::TYPE_UINT32:
    {
        uint32_t      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    case QTBDataValue::TYPE_FLOAT:
    {
        float      val;
        updateValue(timeSec, val);
        return QTBDataValue(val);
    }
    }

    return QTBDataValue();
}

double DemoParameter::signalFrequency() const
{
    return mSignalFrequency;
}

void DemoParameter::setSignalFrequency(double signalFrequency)
{
    mSignalFrequency = signalFrequency;
}

double DemoParameter::signalPhase() const
{
    return mSignalPhase;
}

void DemoParameter::setSignalPhase(double signalPhase)
{
    mSignalPhase = signalPhase;
}

float DemoParameter::signalAmplitude() const
{
    return mSignalAmplitude;
}

void DemoParameter::setSignalAmplitude(float signalAmplitude)
{
    mSignalAmplitude = signalAmplitude;
}

float DemoParameter::signalOffset() const
{
    return mSignalOffset;
}

void DemoParameter::setSignalOffset(float signalOffset)
{
    mSignalOffset = signalOffset;
}

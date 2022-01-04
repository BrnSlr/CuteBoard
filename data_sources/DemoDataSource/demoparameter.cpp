#include <QElapsedTimer>
#include <QDateTime>
#include "demoparameter.h"

DemoParameter::DemoParameter(QString label,
                             int dataSize,
                             int sample_size,
                             QObject *parent) :
    QObject(parent),
    TimeSeries(dataSize, sample_size),
    mGeneratorType(gtSine),
    mValueType(DataValue::TYPE_FLOAT),
    mSignalFrequency(0.1),
    mSignalPhase(0),
    mSignalAmplitude(100),
    mSignalOffset(1000),
    mRefreshPeriod(20),
    mSamplesPerPeriod(10)
{
    setName(label);
}

DemoParameter::GeneratorType DemoParameter::generatorType() const
{
    return mGeneratorType;
}

void DemoParameter::setGeneratorType(const GeneratorType &generatorType)
{
    mGeneratorType = generatorType;
}

DataValue::ValueType DemoParameter::valueType() const
{
    return mValueType;
}

void DemoParameter::setValueType(const DataValue::ValueType &valueType)
{
    mValueType = valueType;
}

DataValue DemoParameter::updateParameter(double timeSec)
{
    switch(mValueType) {
    case DataValue::TYPE_INT8:
    {
        int8_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_UINT8:
    {
        uint8_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_INT16:
    {
        int16_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_UINT16:
    {
        uint16_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_INT32:
    {
        int32_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_UINT32:
    {
        uint32_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_FLOAT:
    {
        float      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_INT64:
    {
        int64_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_UINT64:
    {
        uint64_t      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    case DataValue::TYPE_DOUBLE:
    {
        double      val;
        updateValue(timeSec, val);
        return DataValue(val);
    }
    }

    return DataValue();
}

void DemoParameter::updateParameterVec(double timeSec)
{
    QVector<double> timestamps;
    QVector<DataValue> values;

    if(size() == 1) {
        double startimeSec = timeSec - double(mRefreshPeriod / 1000.0);

        values.resize(mSamplesPerPeriod);
        timestamps.resize(mSamplesPerPeriod);
        double offset = double(mRefreshPeriod / 1000.0) / double(mSamplesPerPeriod);
        for(int i=0; i<mSamplesPerPeriod; i++) {

            values[i] = updateParameter(startimeSec);
            timestamps[i] = startimeSec;
            startimeSec += offset;
        }

        write(timestamps, values);
    } else {


        double startimeSec = timeSec - double(mRefreshPeriod / 1000.0);
        double offsetTime = double(mRefreshPeriod / 1000.0) / double(mSamplesPerPeriod);
        for(int k=0; k<mSamplesPerPeriod; k++) {
            auto vsize = size();
            values.resize(vsize);
            QVector<double> x;

            int oversize = vsize + 100;
            x.resize(oversize);

            for(int i=0; i< oversize; i++) {
                x[i] = (i/double(vsize-1)-0.5)*30+0.01;
            }

            int offset  = QRandomGenerator::global()->bounded(100);
            for(int i=0; i< vsize; i++) {
                values[i] = DataValue(float(qAbs(qSin(x[i+offset])/x[i+offset])));
            }

            write(startimeSec, values);
            startimeSec += offsetTime;
        }
    }

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

double DemoParameter::signalAmplitude() const
{
    return mSignalAmplitude;
}

void DemoParameter::setSignalAmplitude(double signalAmplitude)
{
    mSignalAmplitude = signalAmplitude;
}

double DemoParameter::signalOffset() const
{
    return mSignalOffset;
}

void DemoParameter::setSignalOffset(double signalOffset)
{
    mSignalOffset = signalOffset;
}

int DemoParameter::refreshPeriod() const
{
    return mRefreshPeriod;
}

void DemoParameter::setRefreshPeriod(int refreshPeriod)
{
    mRefreshPeriod = refreshPeriod;
}

int DemoParameter::samplesPerPeriod() const
{
    return mSamplesPerPeriod;
}

void DemoParameter::setSamplesPerPeriod(int samplesPerPeriod)
{
    mSamplesPerPeriod = samplesPerPeriod;
}

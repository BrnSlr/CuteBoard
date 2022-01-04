#include "time_series.h"

TimeSeries::TimeSeries(int nbSamples,
                             int sampleSize):
    TimeSeriesParameter(),
    mValid(true)
{
    setDepth(nbSamples);
    setSize(sampleSize);

    mValues = new (std::nothrow) DataValue[mDepth * mSize]();
    mTimestamps = new (std::nothrow) double[mDepth]();

    if(mValues == nullptr || mTimestamps == nullptr)
        mValid = false;

    mWritePos = 0;
    mReadPos = 0;
}

TimeSeries::~TimeSeries()
{
    delete [] mValues;
    delete [] mTimestamps;
}


bool TimeSeries::valid() const
{
    return mValid;
}

void TimeSeries::write(double t,
                          const DataValue value)
{
    if(mValid) {
        if(mSize == 1) {
            QMutexLocker locker(&mMutex);

            mValues[mWritePos] = value;
            mTimestamps[mWritePos] = t;

            mWritePos = mWritePos + 1 == mDepth ? 0 : mWritePos + 1;
            if(mWritePos == mReadPos)
                mReadPos = mWritePos + 1 == mDepth ? 0 : mWritePos + 1;
        }
    }
}

void TimeSeries::write(double t,
                          const QVector<DataValue> values)
{
    if(mValid) {
        if(values.count() == mSize) {

            QMutexLocker locker(&mMutex);

            int index = 0;
            for(auto v : values) {
                mValues[mWritePos * mSize + index] = v;
                index ++;
            }

            mTimestamps[mWritePos] = t;

            mWritePos = mWritePos + 1 == mDepth ? 0 : mWritePos + 1;
            if(mWritePos == mReadPos)
                mReadPos = mWritePos + 1 == mDepth ? 0 : mWritePos + 1;
        }
    }
}

void TimeSeries::write(const QVector<double> timestamps,
                          const QVector<DataValue> values)
{
    if(mValid) {
        if((timestamps.size() == values.size()) &&
                mSize == 1) {
            QMutexLocker locker(&mMutex);

            QVector<DataValue>::const_iterator itValues;
            QVector<double>::const_iterator itTimestamps;

            for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                 (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                 ++itValues, ++itTimestamps) {

                mValues[mWritePos] = *itValues;
                mTimestamps[mWritePos] = *itTimestamps;

                mWritePos = mWritePos + 1 == mDepth ? 0 : mWritePos + 1;
                if(mWritePos == mReadPos)
                    mReadPos = mWritePos + 1 == mDepth ? 0 : mWritePos + 1;
            }
        }
    }
}

void TimeSeries::read(bool unread,
                         QVector<double> &timestamps,
                         QVector<DataValue> &values)
{
    if(mValid) {
        QMutexLocker locker(&mMutex);
        int pos;
        if(unread) {
            if(mReadPos == mWritePos)
                return;

            auto size = mWritePos > mReadPos ? mWritePos - mReadPos :  mDepth + mWritePos - mReadPos;

            timestamps.resize(size);
            values.resize(size * mSize);
            pos = mReadPos;
        } else {
            timestamps.resize(mDepth);
            values.resize(capacity());
            pos = mWritePos;
        }

        int index = 0;
        for(;;) {
            timestamps[index] = mTimestamps[pos];

            for(int i = 0; i<mSize; i++)
                values[index*mSize + i] = mValues[pos*mSize + i];

            pos = pos + 1 == mDepth ? 0 : pos + 1;
            if(pos == mWritePos)
                break;
            index ++;
        }

        mReadPos = mWritePos;
    }
}

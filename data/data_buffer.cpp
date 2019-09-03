#include "data_buffer.h"

QTBDataBuffer::QTBDataBuffer(quint16 periodMsMax, quint16 historySec) :
    mIndexCount(0),
    mPeriodMsMax(periodMsMax),
    mHistorySec(historySec),
    mHistoryAutoResize(true)
{

}

quint32 QTBDataBuffer::createSerie()
{
    mIndexCount ++;
    mDataSeries.insert(mIndexCount, QTBDataSerie(mPeriodMsMax, mHistorySec));
    return mIndexCount;
}

QTBDataSerie QTBDataBuffer::serie(quint32 serieIndex)
{
    if(mDataSeries.contains(serieIndex))
        return mDataSeries.value(serieIndex);
    
    return QTBDataSerie();
}

void QTBDataBuffer::removeSerie(quint32 serieIndex)
{
    if(mDataSeries.contains(serieIndex))
        mDataSeries.remove(serieIndex);
}

void QTBDataBuffer::addSample(quint32 serieIndex, QTBDataSample sample)
{
    if(mDataSeries.contains(serieIndex)) {
        mDataSeries[serieIndex].add(sample);
        if(mHistoryAutoResize)
            mDataSeries[serieIndex].removeBefore(sample.datationSec() - mHistorySec);
    }
}

void QTBDataBuffer::addSample(quint32 serieIndex, double timestamp, QTBDataValue value)
{
    if(mDataSeries.contains(serieIndex)) {
        mDataSeries[serieIndex].add(QTBDataSample(timestamp, value));
        if(mHistoryAutoResize)
            mDataSeries[serieIndex].removeBefore(timestamp - mHistorySec);
    }
}

QTBDataSample QTBDataBuffer::lastSample(quint32 serieIndex)
{
    if(mDataSeries.contains(serieIndex)) {
        return *(mDataSeries.value(serieIndex).constEnd()-1);
    }
    return {};
}

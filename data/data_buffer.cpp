#include "data_buffer.h"

QTBDataBuffer::QTBDataBuffer() :
    mIndexCount(0)
{

}

quint32 QTBDataBuffer::createSerie()
{
    mIndexCount ++;
    mDataSeries.insert(mIndexCount, QTBDataSerie());
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

void QTBDataBuffer::addSample(quint32 serieIndex, double timestamp, QTBDataValue value)
{
    if(mDataSeries.contains(serieIndex)) {
        mDataSeries[serieIndex].addSample(timestamp, value);
    }
}

QTBDataSample QTBDataBuffer::lastSample(quint32 serieIndex)
{
    if(mDataSeries.contains(serieIndex)) {
        return *(mDataSeries.value(serieIndex).constEnd()-1);
    }
    return {};
}

#ifndef DATABUFFER_H
#define DATABUFFER_H

#include "data_serie.h"

class QTBDataBuffer
{
public:
    QTBDataBuffer();

    quint32 createSerie();
    QTBDataSerie serie(quint32 serieIndex);
    void removeSerie(quint32 serieIndex);
    void addSample(quint32 serieIndex, double timestamp, QTBDataValue value);
    QTBDataSample lastSample(quint32 serieIndex);

private:
    quint32 mIndexCount;
    QHash<quint32, QTBDataSerie> mDataSeries;
    bool mHistoryAutoResize;
};

#endif // DATA_H

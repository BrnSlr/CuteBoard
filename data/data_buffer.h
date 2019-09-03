#ifndef DATABUFFER_H
#define DATABUFFER_H

#include "data_serie.h"

class QTBDataBuffer
{
public:
    QTBDataBuffer(quint16 periodMsMax = DEFAULT_DATA_PERIOD_MS_MAX,
            quint16 historySec = DEFAULT_DATA_HISTORY_SEC);

    quint32 createSerie();
    QTBDataSerie serie(quint32 serieIndex);
    void removeSerie(quint32 serieIndex);

    void addSample(quint32 serieIndex, QTBDataSample sample);
    void addSample(quint32 serieIndex, double timestamp, QTBDataValue value);
    QTBDataSample lastSample(quint32 serieIndex);

private:
    quint32 mIndexCount;
    QHash<quint32, QTBDataSerie> mDataSeries;
    quint16 mPeriodMsMax;
    quint16 mHistorySec;
    bool mHistoryAutoResize;
};

#endif // DATA_H

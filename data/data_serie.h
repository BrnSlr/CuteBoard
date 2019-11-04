#ifndef DATASERIE_H
#define DATASERIE_H

#include "data_sample.h"
#include <QDebug>

#define SERIE_MAX_POINTS 1024

class QTBDataSerie : public QCPDataContainer<QTBDataSample>
{
public:
    QTBDataSerie():
        QCPDataContainer<QTBDataSample>(),
        mHistoryAutoResize(true),
        mCounter(SERIE_MAX_POINTS)
    {
        preallocateGrow(SERIE_MAX_POINTS);
    }

    void addSample(double timestamp, QTBDataValue value)
    {
        QTBDataSample sample(mCounter, timestamp, value);
        mCounter++;

        add(sample);
        removeBefore(mCounter - SERIE_MAX_POINTS);
    }

    void setHistoryAutoResize(bool historyAutoResize)
    {
        mHistoryAutoResize = historyAutoResize;
    }

private:
    bool mHistoryAutoResize;
    quint32 mCounter;
};

Q_DECLARE_TYPEINFO(QTBDataSerie, Q_MOVABLE_TYPE);

#endif // DATASERIE_H

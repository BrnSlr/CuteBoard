#ifndef DATASERIE_H
#define DATASERIE_H

#include "data_sample.h"
#include "data_common.h"

class QTBDataSerie : public QCPDataContainer<QTBDataSample>
{
public:
    QTBDataSerie(quint16 periodMsMax = DEFAULT_DATA_PERIOD_MS_MAX,
                 quint16 historySec = DEFAULT_DATA_HISTORY_SEC):
        QCPDataContainer<QTBDataSample>()
    {
        preallocateGrow(1000 * historySec /periodMsMax );
    }
};

Q_DECLARE_TYPEINFO(QTBDataSerie, Q_MOVABLE_TYPE);

#endif // DATASERIE_H

#ifndef TIMESERIESPARAMETER_H
#define TIMESERIESPARAMETER_H

#include <QString>
#include "data_global.h"

class DATASHARED_EXPORT TimeSeriesParameter
{
public:
    TimeSeriesParameter();

    quint32 parameterId() const;
    QString name() const;
    QString unit() const;
    QString sourceName() const;
    int depth() const;
    int size() const;
    int capacity() const;
    bool isRangeValid();
    double rangeMax() const;
    double rangeMin() const;
    double indexStart() const;
    double indexOffset() const;

    void setParameterId(const quint32 &parameterId);
    void setName(const QString &name);
    void setUnit(const QString &unit);
    void setSourceName(const QString &sourceName);
    void setDepth(int depth);
    void setSize(int size);
    void setRangeMax(double rangeMax);
    void setRangeMin(double rangeMin);
    void setIndexStart(double indexStart);
    void setIndexOffset(double indexOffset);

protected:
    quint32         mParameterId;
    QString         mName;
    QString         mUnit;
    QString         mSourceName;

    int             mDepth;
    int             mSize;

    double          mRangeMax;
    double          mRangeMin;

    double          mIndexStart;
    double          mIndexOffset;

};

#endif // TIMESERIESPARAMETER_H

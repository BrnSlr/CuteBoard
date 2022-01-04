#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include <QString>
#include <QMutex>
#include "time_series_parameter.h"
#include "data_global.h"
#include "data.h"

class DATASHARED_EXPORT TimeSeries : public TimeSeriesParameter
{
public:
    TimeSeries(int nbSamples = DEFAULT_TS_SIZE,
                  int sampleSize = 1);
    ~TimeSeries();

    void write(double t,
               const DataValue value);
    void write(double t,
               const QVector<DataValue> values);
    void write(const QVector<double> timestamps,
               const QVector<DataValue> values);
    void read(bool unread,
              QVector<double> &timestamps,
              QVector<DataValue> &values);

    bool valid() const;

private:
    bool            mValid;

    DataValue *mValues;
    double *mTimestamps;

    QMutex mMutex;
    int mWritePos;
    int mReadPos;
};

#endif // TIME_SERIES_H

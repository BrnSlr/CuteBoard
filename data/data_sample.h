#ifndef DATASAMPLE_H
#define DATASAMPLE_H

#include "3rdparty/qcustomplot.h"
#include "data/data_value.h"

class QTBDataSample
{
public:

    QTBDataSample()
    {
        mCounter = 0.;
        mDatationSec = 0;
        mValue = QTBDataValue();
    }

    QTBDataSample( double datationSec,
                   QTBDataValue val)
    {
        mCounter = 0.;
        mDatationSec = datationSec;
        mValue = val;
    }

    QTBDataSample(double counter,
                  double datationSec,
                  QTBDataValue val)
    {
        mCounter = counter;
        mDatationSec = datationSec;
        mValue = val;
    }

    inline double sortKey() const { return mCounter; }
    inline static QTBDataSample fromSortKey(double sortKey) { return QTBDataSample(sortKey,0, 0); }
    inline static bool sortKeyIsMainKey() { return true; }

    inline double mainKey() const { return mDatationSec; }
    inline double mainValue() { return double(mValue.uint32_value()); }

    inline QTBDataValue value() const { return mValue; }
    inline double datationSec() const { return mDatationSec; }
    inline double datationSecFromMidnight() const { return std::fmod(mDatationSec,86400.0); }

    inline double counter() const { return mCounter; }
    void setCounter(double counter) { mCounter = counter; }
private:
    double mCounter;
    double mDatationSec;
    QTBDataValue mValue;
};

Q_DECLARE_TYPEINFO(QTBDataSample, Q_PRIMITIVE_TYPE);

#endif // DATASAMPLE_H

#ifndef DATASAMPLE_H
#define DATASAMPLE_H

#include "3rdparty/qcustomplot.h"
#include "data/data_value.h"

class QTBDataSample
{
public:

    QTBDataSample() { mDatationSec = 0;
                      mValue = QTBDataValue(); }
    QTBDataSample(double datationSec, QTBDataValue val) { mDatationSec = datationSec;
                                                          mValue = val; }

    inline double sortKey() const { return mDatationSec; }
    inline static QTBDataSample fromSortKey(double sortKey) { return QTBDataSample(sortKey, 0); }
    inline static bool sortKeyIsMainKey() { return true; }

    inline double mainKey() const { return mDatationSec; }
    inline double mainValue() { return double(mValue.uint32_value()); }

    inline QCPRange valueRange() { return QCPRange(mValue.uint32_value(), mValue.uint32_value()); }

    inline QTBDataValue value() const { return mValue; }
    inline double datationSec() const { return mDatationSec; }
    inline double datationSecFromMidnight() const { return std::fmod(mDatationSec,86400.0); }

private:
    double mDatationSec;
    QTBDataValue mValue;
};

Q_DECLARE_TYPEINFO(QTBDataSample, Q_PRIMITIVE_TYPE);

#endif // DATASAMPLE_H

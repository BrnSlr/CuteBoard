#include "thresholdsmapping.h"

QTBThresholdsMapping::QTBThresholdsMapping() :
    mCheckOrder(coLowThresholdsFirst),
    mModified(false)
{

}

QTBThresholdsMapping::~QTBThresholdsMapping()
{
    mLowThresholds.clear();
    mHighThresholds.clear();
}

void QTBThresholdsMapping::insertLowThreshold(double value, const QTBColorSettings& colorSettings)
{
    removeLowThreshold(value);
    mLowThresholds.insert(value, colorSettings);
    mModified = true;
}

void QTBThresholdsMapping::insertHighThreshold(double value, const QTBColorSettings& colorSettings)
{
    removeHighThreshold(value);
    mHighThresholds.insert(value, colorSettings);
    mModified = true;
}

void QTBThresholdsMapping::removeLowThreshold(double value)
{
    mLowThresholds.remove(value);
    mModified = true;
}

void QTBThresholdsMapping::removeHighThreshold(double value)
{
    mHighThresholds.remove(value);
    mModified = true;
}

void QTBThresholdsMapping::clearLowThreshold()
{
    mLowThresholds.clear();
}

void QTBThresholdsMapping::clearHighThreshold()
{
    mHighThresholds.clear();
    mModified = true;
}

QTBColorSettings QTBThresholdsMapping::colorSettings(double value, bool &colorIsSet)
{
    QTBColorSettings cs;
    colorIsSet = false;
    if(mCheckOrder == coHighThresholdsFirst) {
        QMapIterator<double, QTBColorSettings> itHigh(mHighThresholds);
        while (itHigh.hasNext()) {
            itHigh.next();
            if(value > itHigh.key()) {
                cs = itHigh.value();
                colorIsSet = true;
            }
            else {
                break;
            }
        }

        if(!colorIsSet) {
            QMapIterator<double, QTBColorSettings> itLow(mLowThresholds);
            itLow.toBack();
            while (itLow.hasPrevious()) {
                itLow.previous();
                if(value < itLow.key()) {
                    cs = itLow.value();
                    colorIsSet = true;
                }
                else {
                    break;
                }
            }
        }

    } else {
        QMapIterator<double, QTBColorSettings> itLow(mLowThresholds);
        itLow.toBack();
        while (itLow.hasPrevious()) {
            itLow.previous();
            if(value < itLow.key()) {
                cs = itLow.value();
                colorIsSet = true;
            }
            else {
                break;
            }
        }

        if(!colorIsSet) {
            QMapIterator<double, QTBColorSettings> itHigh(mHighThresholds);
            while (itHigh.hasNext()) {
                itHigh.next();
                if(value > itHigh.key()) {
                    cs = itHigh.value();
                    colorIsSet = true;
                }
                else {
                    break;
                }
            }
        }

    }

    return cs;
}

bool QTBThresholdsMapping::active()
{
    if(mHighThresholds.count() > 0 ||
            mLowThresholds.count() > 0)
        return true;
    return false;
}

QTBThresholdsMapping::CheckOrder QTBThresholdsMapping::checkOrder() const
{
    return mCheckOrder;
}

void QTBThresholdsMapping::setCheckOrder(const CheckOrder &checkOrder)
{
    mCheckOrder = checkOrder;
}

QMap<double, QTBColorSettings> QTBThresholdsMapping::highThresholds() const
{
    return mHighThresholds;
}

QMap<double, QTBColorSettings> QTBThresholdsMapping::lowThresholds() const
{
    return mLowThresholds;
}

bool QTBThresholdsMapping::modified() const
{
    return mModified;
}

void QTBThresholdsMapping::setModified(bool modified)
{
    mModified = modified;
}

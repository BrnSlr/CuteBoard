#include "thresholds.h"

Thresholds::Thresholds() :
    mCheckOrder(coLowThresholdsFirst),
    mModified(false)
{

}

Thresholds::~Thresholds()
{
    mLowThresholds.clear();
    mHighThresholds.clear();
}

void Thresholds::insertLowThreshold(double value, const ColorSettings& colorSettings)
{
    removeLowThreshold(value);
    mLowThresholds.insert(value, colorSettings);
    mModified = true;
}

void Thresholds::insertHighThreshold(double value, const ColorSettings& colorSettings)
{
    removeHighThreshold(value);
    mHighThresholds.insert(value, colorSettings);
    mModified = true;
}

void Thresholds::removeLowThreshold(double value)
{
    mLowThresholds.remove(value);
    mModified = true;
}

void Thresholds::removeHighThreshold(double value)
{
    mHighThresholds.remove(value);
    mModified = true;
}

void Thresholds::clearLowThresholds()
{
    mLowThresholds.clear();
}

void Thresholds::clearHighThresholds()
{
    mHighThresholds.clear();
    mModified = true;
}

ColorSettings Thresholds::colorSettings(double value, bool &colorIsSet)
{
    ColorSettings cs;
    colorIsSet = false;
    if(mCheckOrder == coHighThresholdsFirst) {
        QMapIterator<double, ColorSettings> itHigh(mHighThresholds);
        while (itHigh.hasNext()) {
            itHigh.next();
            if(value >= itHigh.key()) {
                cs = itHigh.value();
                colorIsSet = true;
            }
            else {
                break;
            }
        }

        if(!colorIsSet) {
            QMapIterator<double, ColorSettings> itLow(mLowThresholds);
            itLow.toBack();
            while (itLow.hasPrevious()) {
                itLow.previous();
                if(value <= itLow.key()) {
                    cs = itLow.value();
                    colorIsSet = true;
                }
                else {
                    break;
                }
            }
        }

    } else {
        QMapIterator<double, ColorSettings> itLow(mLowThresholds);
        itLow.toBack();
        while (itLow.hasPrevious()) {
            itLow.previous();
            if(value <= itLow.key()) {
                cs = itLow.value();
                colorIsSet = true;
            }
            else {
                break;
            }
        }

        if(!colorIsSet) {
            QMapIterator<double, ColorSettings> itHigh(mHighThresholds);
            while (itHigh.hasNext()) {
                itHigh.next();
                if(value >= itHigh.key()) {
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

bool Thresholds::active()
{
    if(mHighThresholds.count() > 0 ||
            mLowThresholds.count() > 0)
        return true;
    return false;
}

Thresholds::CheckOrder Thresholds::checkOrder() const
{
    return mCheckOrder;
}

void Thresholds::setCheckOrder(const CheckOrder &checkOrder)
{
    mCheckOrder = checkOrder;
}

QMap<double, ColorSettings> Thresholds::highThresholds() const
{
    return mHighThresholds;
}

QMap<double, ColorSettings> Thresholds::lowThresholds() const
{
    return mLowThresholds;
}

bool Thresholds::modified() const
{
    return mModified;
}

void Thresholds::setModified(bool modified)
{
    mModified = modified;
}

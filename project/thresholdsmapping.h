#ifndef THRESHOLDSSETTINGS_H
#define THRESHOLDSSETTINGS_H

#include "colorsettings.h"

class QTBThresholdsMapping
{
public:
    enum CheckOrder {
        coLowThresholdsFirst,
        coHighThresholdsFirst
    };

    QTBThresholdsMapping();
    ~QTBThresholdsMapping();

    void insertLowThreshold(double value, const QTBColorSettings& colorSettings);
    void insertHighThreshold(double value, const QTBColorSettings& colorSettings);

    void removeLowThreshold(double value);
    void removeHighThreshold(double value);

    void clearLowThreshold();
    void clearHighThreshold();

    QTBColorSettings colorSettings(double value, bool &colorIsSet);

    bool active();

    CheckOrder checkOrder() const;
    void setCheckOrder(const CheckOrder &checkOrder);

    QMap<double, QTBColorSettings> highThresholds() const;
    QMap<double, QTBColorSettings> lowThresholds() const;

    bool modified() const;
    void setModified(bool modified);

protected:
    CheckOrder mCheckOrder;
    QMap<double, QTBColorSettings> mHighThresholds;
    QMap<double, QTBColorSettings> mLowThresholds;
    bool mModified;
};

#endif // THRESHOLDSSETTINGS_H

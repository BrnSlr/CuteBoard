#ifndef THRESHOLDS_H
#define THRESHOLDS_H

#include "color_settings.h"
#include "ui/ui_global.h"

class BOARD_UI_EXPORT Thresholds
{
public:
    enum CheckOrder {
        coLowThresholdsFirst,
        coHighThresholdsFirst
    };

    Thresholds();
    ~Thresholds();

    void insertLowThreshold(double value, const ColorSettings& colorSettings);
    void insertHighThreshold(double value, const ColorSettings& colorSettings);

    void removeLowThreshold(double value);
    void removeHighThreshold(double value);

    void clearLowThresholds();
    void clearHighThresholds();

    ColorSettings colorSettings(double value, bool &colorIsSet);

    bool active();

    CheckOrder checkOrder() const;
    void setCheckOrder(const CheckOrder &checkOrder);

    QMap<double, ColorSettings> highThresholds() const;
    QMap<double, ColorSettings> lowThresholds() const;

    bool modified() const;
    void setModified(bool modified);

protected:
    CheckOrder mCheckOrder;
    QMap<double, ColorSettings> mHighThresholds;
    QMap<double, ColorSettings> mLowThresholds;
    bool mModified;
};

#endif // THRESHOLDS_H

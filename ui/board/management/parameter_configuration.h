#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QString>
#include <QSettings>
#include "3rdparty/qcustomplot/qcustomplot.h"
#include "color_settings.h"
#include "thresholds.h"
#include "states.h"
#include "bitfield.h"
#include "ui/ui_global.h"

class BOARD_UI_EXPORT ParameterConfiguration : public QSharedData
{
public:

    enum ConfigurationMode {
        cmFull,
        cmValue,
        cmState,
        cmBitFields,
        cmCurveX,
        cmCurveY,
        cmGraph
    };

    enum ItemColor {
        icBase,
        icDynamic,
        icDynamicSegmented,
        icCustom
    };    

    enum BrushStyle {
        bsNone,
        bsFilled,
        bsGradient
    };

    ParameterConfiguration();

    void saveToFile(const QString& saveDirectory);
    void loadFromFile(const QString& settingsFile);

    void save(QSettings *settings, ConfigurationMode mode = cmFull);
    void load(QSettings *settings, ConfigurationMode mode = cmFull);

    int precision() const;
    QString description() const;
    QString userDefinedLabel() const;
    QString userDefinedUnit() const;
    bool validRange() const;
    double rangeMaximum() const;
    double rangeMinimum() const;
    QString label() const;
    bool userLabelEnabled() const;
    bool userUnitEnabled() const;
    bool outOfRangeColorEnabled() const;
    BrushStyle graphBrush() const;
    QCPGraph::LineStyle graphLineStyle() const;
    double penWidth() const;
    QCPScatterStyle::ScatterShape scatterShape() const;
    int scatterSize() const;
    QCPScatterStyle scatterStyle() const;
    QCPCurve::LineStyle curveLineStyle() const;
    bool itemsThresholdsVisible() const;
    bool curveTracerVisible() const;
    bool modified();
    QColor itemStaticColor() const;
    ItemColor itemColorMode() const;

    void setPrecision(int precision);
    void setDescription(const QString &description);
    void setUserDefinedLabel(const QString &userDefinedLabel);
    void setUserDefinedUnit(const QString &userDefinedUnit);
    void setValidRange(bool validRange);
    void setRangeMaximum(double rangeMaximum);
    void setRangeMinimum(double rangeMinimum);
    void setLabel(const QString &label);
    void setUserLabelEnabled(bool userLabelEnabled);
    void setUserUnitEnabled(bool userUnitEnabled);
    void setOutOfRangeColorEnabled(bool outOfRangeColorEnabled);
    void setGraphBrush(BrushStyle graphBrush);
    void setGraphLineStyle(const QCPGraph::LineStyle &graphLineStyle);
    void setPenWidth(double penWidth);
    void setScatterShape(const QCPScatterStyle::ScatterShape &scatterShape);
    void setScatterSize(int scatterSize);
    void setScatterStyle(const QCPScatterStyle &scatterStyle);
    void setCurveLineStyle(const QCPCurve::LineStyle &curveLineStyle);
    void setCurveThresholdsVisible(bool curveThresholdsVisible);
    void setGraphThresholdsVisible(bool graphThresholdsVisible);
    void setItemsThresholdsVisible(bool itemsThresholdsVisible);
    void setCurveTracerVisible(bool visible);
    void setModified(bool modified);
    void setItemStaticColor(const QColor &itemStaticColor);
    void setItemColorMode(const ItemColor &itemColorMode);

    ColorSettings &defaultColorSettingsRef();
    Thresholds &thresholdsSettingsRef();
    ColorSettings &outOfRangeColorSettingsRef();
    States &statesSettingsRef();
    Bitfield &bitfieldsSettingsRef();

    QString saveFilePath() const;

protected:
    QString mSaveFilePath;
    int mPrecision;
    QString mLabel;
    QString mDescription;
    QString mUserDefinedLabel;
    bool mUserLabelEnabled;
    QString mUserDefinedUnit;
    bool mUserUnitEnabled;
    bool mValidRange;
    double mRangeMaximum;
    double mRangeMinimum;
    bool mOutOfRangeColorEnabled;

    bool mItemsThresholdsVisible;

    QColor mItemStaticColor;
    ItemColor mItemColorMode;
    double mPenWidth;

    QCPGraph::LineStyle mGraphLineStyle;
    BrushStyle mGraphBrush;

    QCPCurve::LineStyle mCurveLineStyle;
    QCPScatterStyle::ScatterShape mScatterShape;
    int mScatterSize;
    QCPScatterStyle mScatterStyle;
    bool mCurveTracerVisible;

    ColorSettings mOutOfRangeColorSettings;
    ColorSettings mDefaultColorSettings;
    Thresholds mThresholdsSettings;
    States mStatesSettings;
    Bitfield mBitfieldsSettings;

    bool mModified;
};

#endif // PROPERTIES_H

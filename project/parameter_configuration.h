#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QString>
#include <QSettings>
#include "3rdparty/qcustomplot.h"
#include "project/colorsettings.h"
#include "project/thresholdsmapping.h"
#include "project/statesmapping.h"
#include "project/bitfieldsmapping.h"

class QTBParameterConfiguration : public QSharedData
{
public:

    enum ConfigurationModule {
        cmFull,
        cmValue,
        cmState,
        cmBitFields,
        cmCurve
    };

    QTBParameterConfiguration();

    void saveToFile(const QString& saveDirectory);
    void loadFromFile(const QString& settingsFile);

    void save(QSettings *settings, ConfigurationModule mode = cmFull);
    void load(QSettings *settings, ConfigurationModule mode = cmFull);

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
    bool graphBrush() const;
    QCPGraph::LineStyle graphLineStyle() const;
    QCPScatterStyle::ScatterShape scatterShape() const;
    int scatterSize() const;
    QCPScatterStyle scatterStyle() const;
    QCPCurve::LineStyle curveLineStyle() const;
    bool itemsThresholdsVisible() const;
    bool curveTracerVisible() const;
    bool modified();

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
    void setGraphBrush(bool graphBrush);
    void setGraphLineStyle(const QCPGraph::LineStyle &graphLineStyle);
    void setScatterShape(const QCPScatterStyle::ScatterShape &scatterShape);
    void setScatterSize(int scatterSize);
    void setScatterStyle(const QCPScatterStyle &scatterStyle);
    void setCurveLineStyle(const QCPCurve::LineStyle &curveLineStyle);
    void setCurveThresholdsVisible(bool curveThresholdsVisible);
    void setGraphThresholdsVisible(bool graphThresholdsVisible);
    void setItemsThresholdsVisible(bool itemsThresholdsVisible);
    void setCurveTracerVisible(bool visible);
    void setModified(bool modified);

    QTBColorSettings &defaultColorSettingsRef();
    QTBThresholdsMapping &thresholdsSettingsRef();
    QTBColorSettings &outOfRangeColorSettingsRef();
    QTBStatesMapping &statesSettingsRef();
    QTBBitfieldsMapping &bitfieldsSettingsRef();


protected:
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

    QCPGraph::LineStyle mGraphLineStyle;
    bool mGraphBrush;

    QCPCurve::LineStyle mCurveLineStyle;
    QCPScatterStyle::ScatterShape mScatterShape;
    int mScatterSize;
    QCPScatterStyle mScatterStyle;
    bool mCurveTracerVisible;

    QTBColorSettings mOutOfRangeColorSettings;
    QTBColorSettings mDefaultColorSettings;
    QTBThresholdsMapping mThresholdsSettings;
    QTBStatesMapping mStatesSettings;
    QTBBitfieldsMapping mBitfieldsSettings;

    bool mModified;
};

#endif // PROPERTIES_H

#include "parameter_configuration.h"
#include "project/string_util.h"
#include <QDir>
#include <QSettings>


QTBParameterConfiguration::QTBParameterConfiguration() :
    mPrecision(2),
    mUserLabelEnabled(false),
    mUserUnitEnabled(false),
    mValidRange(false),
    mRangeMaximum(1000),
    mRangeMinimum(-1000),
    mItemsThresholdsVisible(true),
    mGraphLineStyle(QCPGraph::lsLine),
    mGraphBrush(BrushStyle::bsGradient),
    mCurveLineStyle(QCPCurve::lsLine),
    mScatterShape(QCPScatterStyle::ssNone),
    mScatterSize(10),
    mScatterStyle(mScatterShape, mScatterSize),
    mCurveTracerVisible(true),
    mModified(false)
{
    mOutOfRangeColorSettings.setMode(QTBColorSettings::cmFilledBackground);
    mOutOfRangeColorSettings.setColor(Qt::darkRed);
    mDefaultColorSettings.setMode(QTBColorSettings::cmAuto);
    mDefaultColorSettings.setColor(QColor(QString("#8AB8FF")));
    mItemStaticColor = QColor(QString("#8AB8FF"));
    mItemColorMode = icBase;
}

void QTBParameterConfiguration::saveToFile(const QString& saveDirectory)
{
    QString name = mLabel;
    name = removeSpecialChars(name);
    QString descr = mDescription;
    descr = removeSpecialChars(descr);
    QString settingsPath = saveDirectory + QDir::separator() + name + QString( "_" ) + descr  + QString(".param");

    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.clear();
    settings.sync();

    save(&settings);
}

void QTBParameterConfiguration::loadFromFile(const QString& settingsFile)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    load(&settings);
}

void QTBParameterConfiguration::save(QSettings *settings, ConfigurationMode mode)
{
    settings->setValue("Description", mDescription);
    settings->setValue("Label", mLabel);
    settings->setValue("UserDefinedLabel", mUserDefinedLabel);
    settings->setValue("UserLabelEnabled", mUserLabelEnabled);
    settings->setValue("UserDefinedUnit", mUserDefinedUnit);
    settings->setValue("UserUnitEnabled", mUserUnitEnabled);

    settings->setValue("DefaultColor", mDefaultColorSettings.color().name());
    settings->setValue("DefaultColorMode", mDefaultColorSettings.mode());

    if(mode != cmState && mode != cmBitFields) {
        settings->setValue("Precision", mPrecision);
        settings->setValue("ThresholdsVisible", mItemsThresholdsVisible);
        settings->setValue("ItemColorMode", mItemColorMode);
        settings->setValue("ItemColor", mItemStaticColor.name());

        settings->setValue("ValidRange", mValidRange);
        settings->setValue("RangeMaximum", mRangeMaximum);
        settings->setValue("RangeMinimum", mRangeMinimum);

        settings->setValue("OutOfRange", mOutOfRangeColorEnabled);
        settings->setValue("OutOfRangeColor", mOutOfRangeColorSettings.color().name());
        settings->setValue("OutOfRangeColorMode", mOutOfRangeColorSettings.mode());

        if(mode == cmCurveY) {
            settings->beginGroup("Plottable");
            settings->setValue("ScatterShape", mScatterShape);
            settings->setValue("ScatterSize", mScatterSize);

            settings->beginGroup("Curve");
            settings->setValue("LineStyle", mCurveLineStyle);
            settings->setValue("TracerVisible", mCurveTracerVisible);
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmGraph ) {
            settings->beginGroup("Plottable");
            settings->setValue("ScatterShape", mScatterShape);
            settings->setValue("ScatterSize", mScatterSize);

            settings->beginGroup("Graph");
            settings->setValue("Brush", mGraphBrush);
            settings->setValue("LineStyle", mGraphLineStyle);
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmFull ) {
            settings->beginGroup("Plottable");
            settings->setValue("ScatterShape", mScatterShape);
            settings->setValue("ScatterSize", mScatterSize);

            settings->beginGroup("Curve");
            settings->setValue("LineStyle", mCurveLineStyle);
            settings->setValue("TracerVisible", mCurveTracerVisible);
            settings->endGroup();

            settings->beginGroup("Graph");
            settings->setValue("Brush", mGraphBrush);
            settings->setValue("LineStyle", mGraphLineStyle);
            settings->endGroup();

            settings->endGroup();
        }

        settings->beginGroup("Thresholds");
        {
            settings->beginWriteArray("LowMapping");
            {
                int lowCount = 0;
                QMapIterator<double, QTBColorSettings> itLow(mThresholdsSettings.lowThresholds());
                while (itLow.hasNext()) {
                    itLow.next();

                    settings->setArrayIndex(lowCount);
                    settings->setValue("Color", itLow.value().color().name());
                    settings->setValue("Mode", itLow.value().mode());
                    settings->setValue("Value", itLow.key());
                    lowCount ++;

                }
            }
            settings->endArray();

            settings->beginWriteArray("HighMapping");
            {
                int highCount = 0;
                QMapIterator<double, QTBColorSettings> itHigh(mThresholdsSettings.highThresholds());
                while (itHigh.hasNext()) {
                    itHigh.next();

                    settings->setArrayIndex(highCount);
                    settings->setValue("Color", itHigh.value().color().name());
                    settings->setValue("Mode", itHigh.value().mode());
                    settings->setValue("Value", itHigh.key());
                    highCount ++;

                }
            }
            settings->endArray();
        }
        settings->endGroup();
    }

    if(mode == cmState || mode == cmFull){
        settings->beginGroup("States");
        {
            settings->beginWriteArray("Mapping");
            {
                int statesCount = 0;

                QMapIterator<qlonglong, QTBColorSettings> itStateColor(mStatesSettings.statesColor());
                while (itStateColor.hasNext()) {
                    itStateColor.next();

                    settings->setArrayIndex(statesCount);
                    settings->setValue("Active", mStatesSettings.statesActive().value(itStateColor.key()));
                    settings->setValue("Color", itStateColor.value().color().name());
                    settings->setValue("Mode", itStateColor.value().mode());
                    settings->setValue("Value", itStateColor.key());
                    settings->setValue("Text", mStatesSettings.statesText().value(itStateColor.key()));
                    statesCount ++;
                }
            }
            settings->endArray();
        }
        settings->endGroup();
    }

    if(mode == cmBitFields || mode == cmFull){
        settings->beginGroup("Bitfields");
        {
            settings->beginWriteArray("Mapping");
            for(int i=0;i<32;i++) {

                settings->setArrayIndex(i);
                settings->setValue("Logic", mBitfieldsSettings.bitLogics().at(i));
                settings->setValue("Description", mBitfieldsSettings.bitDescriptions().at(i));
            }
            settings->endArray();
        }
        settings->endGroup();
    }
}

void QTBParameterConfiguration::load(QSettings *settings, ConfigurationMode mode)
{
    mLabel= settings->value("Label").toString();
    mDescription= settings->value("Description").toString();
    mUserDefinedLabel= settings->value("UserDefinedLabel").toString();
    mUserLabelEnabled= settings->value("UserLabelEnabled").toBool();
    mUserDefinedUnit= settings->value("UserDefinedUnit").toString();
    mUserUnitEnabled= settings->value("UserUnitEnabled").toBool();

    mDefaultColorSettings.setColor(QColor(settings->value("DefaultColor").toString()));
    mDefaultColorSettings.setMode(QTBColorSettings::ColorMode(settings->value("DefaultColorMode").toInt()));

    if(mode != cmState && mode != cmBitFields) {
        mPrecision = settings->value("Precision").toInt();
        mItemsThresholdsVisible = settings->value("ThresholdsVisible").toBool();
        mItemColorMode = ItemColor(settings->value("ItemColorMode").toInt());
        mItemStaticColor = QColor(settings->value("ItemColor").toString());

        mValidRange= settings->value("ValidRange").toBool();
        mRangeMaximum= settings->value("RangeMaximum").toDouble();
        mRangeMinimum= settings->value("RangeMinimum").toDouble();

        mOutOfRangeColorEnabled= settings->value("OutOfRange").toBool();

        mOutOfRangeColorSettings.setColor(QColor(settings->value("OutOfRangeColor").toString()));
        mOutOfRangeColorSettings.setMode(QTBColorSettings::ColorMode(settings->value("OutOfRangeColorMode").toInt()));


        if(mode == cmFull) {
            settings->beginGroup("Plottable");

            setScatterShape(QCPScatterStyle::ScatterShape(settings->value("ScatterShape").toInt()));
            setScatterSize(settings->value("ScatterSize").toInt());

            settings->beginGroup("Graph");
            mGraphBrush = BrushStyle(settings->value("Brush").toInt());
            mGraphLineStyle = QCPGraph::LineStyle(settings->value("LineStyle").toInt());
            settings->endGroup();

            settings->beginGroup("Curve");
            mCurveLineStyle = QCPCurve::LineStyle(settings->value("LineStyle").toInt());
            mCurveTracerVisible = settings->value("TracerVisible").toBool();
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmCurveY) {
            settings->beginGroup("Plottable");

            setScatterShape(QCPScatterStyle::ScatterShape(settings->value("ScatterShape").toInt()));
            setScatterSize(settings->value("ScatterSize").toInt());

            settings->beginGroup("Curve");
            mCurveLineStyle = QCPCurve::LineStyle(settings->value("LineStyle").toInt());
            mCurveTracerVisible = settings->value("TracerVisible").toBool();
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmGraph) {
            settings->beginGroup("Plottable");

            setScatterShape(QCPScatterStyle::ScatterShape(settings->value("ScatterShape").toInt()));
            setScatterSize(settings->value("ScatterSize").toInt());

            settings->beginGroup("Graph");
            mGraphBrush = BrushStyle(settings->value("Brush").toInt());
            mGraphLineStyle = QCPGraph::LineStyle(settings->value("LineStyle").toInt());
            settings->endGroup();

            settings->endGroup();
        }

        settings->beginGroup("Thresholds");
        {
            mThresholdsSettings.clearLowThreshold();
            int lowThresholdsCount = settings->beginReadArray("LowMapping");
            {
                for(int i=0;i<lowThresholdsCount;i++) {
                    settings->setArrayIndex(i);

                    QTBColorSettings cs;
                    cs.setMode(QTBColorSettings::ColorMode(settings->value("Mode").toInt()));
                    cs.setColor(QColor(settings->value("Color").toString()));
                    mThresholdsSettings.insertLowThreshold(settings->value("Value").toDouble(), cs);
                }
            }
            settings->endArray();

            mThresholdsSettings.clearHighThreshold();
            int highThresholdsCount = settings->beginReadArray("HighMapping");
            {
                for(int i=0;i<highThresholdsCount;i++) {
                    settings->setArrayIndex(i);

                    QTBColorSettings cs;
                    cs.setMode(QTBColorSettings::ColorMode(settings->value("Mode").toInt()));
                    cs.setColor(QColor(settings->value("Color").toString()));
                    mThresholdsSettings.insertHighThreshold(settings->value("Value").toDouble(), cs);
                }
            }
            settings->endArray();
        }
        settings->endGroup();
    }

    if(mode == cmState || mode == cmFull){
        settings->beginGroup("States");
        {
            mStatesSettings.clearStates();
            int statesCount = settings->beginReadArray("Mapping");
            {
                for(int i=0;i<statesCount;i++) {
                    settings->setArrayIndex(i);

                    QTBColorSettings cs;
                    cs.setMode(QTBColorSettings::ColorMode(settings->value("Mode").toInt()));
                    cs.setColor(QColor(settings->value("Color").toString()));

                    mStatesSettings.addState(settings->value("Active").toBool(),
                                             settings->value("Value").toLongLong(),
                                             settings->value("Text").toString(),
                                             cs);
                }
            }
            settings->endArray();
        }
        settings->endGroup();
    }

    if(mode == cmBitFields || mode == cmFull){
        settings->beginGroup("Bitfields");
        {
            int bits = settings->beginReadArray("Mapping");
            {
                for(int i=0;i<bits;i++) {
                    settings->setArrayIndex(i);
                    mBitfieldsSettings.setBitLogic(i, settings->value("Logic").toBool());
                    mBitfieldsSettings.setBitDescription(i, settings->value("Description").toString());
                }
            }
            settings->endArray();
        }
        settings->endGroup();
    }
}

int QTBParameterConfiguration::precision() const
{
    return mPrecision;
}

void QTBParameterConfiguration::setPrecision(int precision)
{
    if(precision != mPrecision) {
        mPrecision = precision;
        mModified = true;
    }
}

QString QTBParameterConfiguration::description() const
{
    return mDescription;
}

void QTBParameterConfiguration::setDescription(const QString &description)
{
    if(description != mDescription) {
        mDescription = description;
        mModified = true;
    }
}

QString QTBParameterConfiguration::userDefinedLabel() const
{
    return mUserDefinedLabel;
}

void QTBParameterConfiguration::setUserDefinedLabel(const QString &userDefinedLabel)
{
    if(userDefinedLabel != mUserDefinedLabel) {
        mUserDefinedLabel = userDefinedLabel;
        mModified = true;
    }
}

QString QTBParameterConfiguration::userDefinedUnit() const
{
    return mUserDefinedUnit;
}

void QTBParameterConfiguration::setUserDefinedUnit(const QString &userDefinedUnit)
{
    if(userDefinedUnit != mUserDefinedUnit) {
        mUserDefinedUnit = userDefinedUnit;
        mModified = true;
    }
}

bool QTBParameterConfiguration::validRange() const
{
    return mValidRange;
}

void QTBParameterConfiguration::setValidRange(bool validRange)
{
    if(validRange != mValidRange) {
        mValidRange = validRange;
        mModified = true;
    }
}

double QTBParameterConfiguration::rangeMaximum() const
{
    return mRangeMaximum;
}

void QTBParameterConfiguration::setRangeMaximum(double rangeMaximum)
{
    if(!qFuzzyCompare(rangeMaximum,mRangeMaximum)) {
        mRangeMaximum = rangeMaximum;
        mModified = true;
    }
}

double QTBParameterConfiguration::rangeMinimum() const
{
    return mRangeMinimum;
}

void QTBParameterConfiguration::setRangeMinimum(double rangeMinimum)
{
    if(!qFuzzyCompare(rangeMinimum,mRangeMinimum)) {
        mRangeMinimum = rangeMinimum;
        mModified = true;
    }
}

QString QTBParameterConfiguration::label() const
{
    return mLabel;
}

void QTBParameterConfiguration::setLabel(const QString &label)
{
    if(label != mLabel) {
        mLabel = label;
        mModified = true;
    }
}

bool QTBParameterConfiguration::userLabelEnabled() const
{
    return mUserLabelEnabled;
}

void QTBParameterConfiguration::setUserLabelEnabled(bool userLabelEnabled)
{
    if(userLabelEnabled != mUserLabelEnabled) {
        mUserLabelEnabled = userLabelEnabled;
        mModified = true;
    }
}

bool QTBParameterConfiguration::userUnitEnabled() const
{
    return mUserUnitEnabled;
}

void QTBParameterConfiguration::setUserUnitEnabled(bool userUnitEnabled)
{
    if(userUnitEnabled != mUserUnitEnabled) {
        mUserUnitEnabled = userUnitEnabled;
        mModified = true;
    }
}

QTBColorSettings &QTBParameterConfiguration::defaultColorSettingsRef()
{
    return mDefaultColorSettings;
}

QTBThresholdsMapping &QTBParameterConfiguration::thresholdsSettingsRef()
{
    return mThresholdsSettings;
}

QTBColorSettings &QTBParameterConfiguration::outOfRangeColorSettingsRef()
{
    return mOutOfRangeColorSettings;
}

bool QTBParameterConfiguration::outOfRangeColorEnabled() const
{
    return mOutOfRangeColorEnabled;
}

void QTBParameterConfiguration::setOutOfRangeColorEnabled(bool outOfRangeColorEnabled)
{
    mOutOfRangeColorEnabled = outOfRangeColorEnabled;
}

QTBStatesMapping &QTBParameterConfiguration::statesSettingsRef()
{
    return mStatesSettings;
}

QTBBitfieldsMapping &QTBParameterConfiguration::bitfieldsSettingsRef()
{
    return mBitfieldsSettings;
}

QTBParameterConfiguration::ItemColor QTBParameterConfiguration::itemColorMode() const
{
    return mItemColorMode;
}

void QTBParameterConfiguration::setItemColorMode(const ItemColor &itemColorMode)
{
    if(itemColorMode != mItemColorMode) {
        mItemColorMode = itemColorMode;
        mModified = true;
    }
}

QColor QTBParameterConfiguration::itemStaticColor() const
{
    return mItemStaticColor;
}

void QTBParameterConfiguration::setItemStaticColor(const QColor &itemStaticColor)
{
    if(itemStaticColor != mItemStaticColor) {
        mItemStaticColor = itemStaticColor;
        mModified = true;
    }
}

bool QTBParameterConfiguration::curveTracerVisible() const
{
    return mCurveTracerVisible;
}

void QTBParameterConfiguration::setCurveTracerVisible(bool visible)
{
    if(visible != mCurveTracerVisible) {
        mCurveTracerVisible = visible;
        mModified = true;
    }
}

bool QTBParameterConfiguration::modified()
{
    mModified = mModified || mThresholdsSettings.modified();
    mModified = mModified || mStatesSettings.modified();
    mModified = mModified || mBitfieldsSettings.modified();
    mModified = mModified || mOutOfRangeColorSettings.modified();
    mModified = mModified || mDefaultColorSettings.modified();
    return mModified;
}

void QTBParameterConfiguration::setModified(bool modified)
{
    mModified = modified;
    mThresholdsSettings.setModified(modified);
    mStatesSettings.setModified(modified);
    mBitfieldsSettings.setModified(modified);
    mOutOfRangeColorSettings.setModified(modified);
    mDefaultColorSettings.setModified(modified);
}

bool QTBParameterConfiguration::itemsThresholdsVisible() const
{
    return mItemsThresholdsVisible;
}

void QTBParameterConfiguration::setItemsThresholdsVisible(bool itemsThresholdsVisible)
{
    if(itemsThresholdsVisible != mItemsThresholdsVisible) {
        mItemsThresholdsVisible = itemsThresholdsVisible;
        mModified = true;
    }
}

QTBParameterConfiguration::BrushStyle QTBParameterConfiguration::graphBrush() const
{
    return mGraphBrush;
}

void QTBParameterConfiguration::setGraphBrush(BrushStyle graphBrush)
{
    if(graphBrush != mGraphBrush) {
        mGraphBrush = graphBrush;
        mModified = true;
    }
}

QCPGraph::LineStyle QTBParameterConfiguration::graphLineStyle() const
{
    return mGraphLineStyle;
}

void QTBParameterConfiguration::setGraphLineStyle(const QCPGraph::LineStyle &graphLineStyle)
{
    if(graphLineStyle != mGraphLineStyle) {
        if(graphLineStyle == QCPGraph::lsStepLeft)
            mGraphLineStyle = QCPGraph::lsImpulse;
        else
            mGraphLineStyle = graphLineStyle;
        mModified = true;

        if(mGraphLineStyle == QCPGraph::lsNone)
            setCurveLineStyle(QCPCurve::lsNone);
        else
            setCurveLineStyle(QCPCurve::lsLine);
    }
}

QCPScatterStyle::ScatterShape QTBParameterConfiguration::scatterShape() const
{
    return mScatterShape;
}

void QTBParameterConfiguration::setScatterShape(const QCPScatterStyle::ScatterShape &scatterShape)
{
    if(scatterShape != mScatterShape) {
        mScatterShape = scatterShape;
        mScatterStyle.setShape(mScatterShape);
        mModified = true;
    }
}

int QTBParameterConfiguration::scatterSize() const
{
    return mScatterSize;
}

void QTBParameterConfiguration::setScatterSize(int scatterSize)
{
    if(scatterSize != mScatterSize) {
        mScatterSize = scatterSize;
        mScatterStyle.setSize(mScatterSize);
        mModified = true;
    }
}

QCPScatterStyle QTBParameterConfiguration::scatterStyle() const
{
    return mScatterStyle;
}

QCPCurve::LineStyle QTBParameterConfiguration::curveLineStyle() const
{
    return mCurveLineStyle;
}

void QTBParameterConfiguration::setCurveLineStyle(const QCPCurve::LineStyle &curveLineStyle)
{
    if(curveLineStyle != mCurveLineStyle) {
        mCurveLineStyle = curveLineStyle;
        mModified = true;
    }
}

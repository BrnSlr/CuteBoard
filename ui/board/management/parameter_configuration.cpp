#include "parameter_configuration.h"
#include "ui/board/management/utils/string_util.h"
#include <QDir>
#include <QSettings>
#include "log4qt/logger.h"
#include "log4qt/logmanager.h"

LOG4QT_DECLARE_STATIC_LOGGER(logger, ParameterConfiguration)

ParameterConfiguration::ParameterConfiguration() :
    mSaveFilePath(QString("")),
    mPrecision(1),
    mUserLabelEnabled(false),
    mUserUnitEnabled(false),
    mValidRange(false),
    mRangeMaximum(1000),
    mRangeMinimum(-1000),
    mItemsThresholdsVisible(true),
    mPenWidth(0.),
    mGraphLineStyle(QCPGraph::lsLine),
    mGraphBrush(BrushStyle::bsNone),
    mCurveLineStyle(QCPCurve::lsLine),
    mScatterShape(QCPScatterStyle::ssNone),
    mScatterSize(10),
    mScatterStyle(mScatterShape, mScatterSize),
    mCurveTracerVisible(true),
    mModified(false)
{
    mOutOfRangeColorSettings.setMode(ColorSettings::cmFilledBackground);
    mOutOfRangeColorSettings.setColor(Qt::darkRed);
    mDefaultColorSettings.setMode(ColorSettings::cmFilledBackground);
    mDefaultColorSettings.setColor(QColor(QString("#8AB8FF")));
    mItemStaticColor = QColor(QString("#8AB8FF"));
    mItemColorMode = icDynamic;
}

void ParameterConfiguration::saveToFile(const QString& saveDirectory)
{
    if(mSaveFilePath.isEmpty()) {
        QString name = mLabel;
        name = removeSpecialChars(name);
        QString descr = mDescription;
        descr = removeSpecialChars(descr);

        mSaveFilePath = saveDirectory + QDir::separator() + name + QString( "_" ) + descr  + QString(".param");
    }

    logger()->info() << QString("Saving ParameterConfiguration [%1][%2] to file [%3]").arg(mLabel).arg(mDescription).arg(mSaveFilePath);

    QSettings settings(mSaveFilePath, QSettings::IniFormat);
    settings.clear();
    settings.sync();

    save(&settings);
}

void ParameterConfiguration::loadFromFile(const QString& settingsFile)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    load(&settings);

    mSaveFilePath = settingsFile;
}

void ParameterConfiguration::save(QSettings *settings, ConfigurationMode mode)
{
    settings->setValue("Description", mDescription);
    settings->setValue("Label", mLabel);
    settings->setValue("UserDefinedLabel", mUserDefinedLabel);
    settings->setValue("UserLabelEnabled", mUserLabelEnabled);
    settings->setValue("UserDefinedUnit", mUserDefinedUnit);
    settings->setValue("UserUnitEnabled", mUserUnitEnabled);

    settings->setValue("DefaultColor", mDefaultColorSettings.color().name(QColor::HexArgb));
    settings->setValue("DefaultColorMode", mDefaultColorSettings.mode());

    if(mode != cmState && mode != cmBitFields) {
        settings->setValue("Precision", mPrecision);
        settings->setValue("ThresholdsVisible", mItemsThresholdsVisible);
        settings->setValue("ItemColorMode", mItemColorMode);
        settings->setValue("ItemColor", mItemStaticColor.name(QColor::HexArgb));

        settings->setValue("ValidRange", mValidRange);
        settings->setValue("RangeMaximum", mRangeMaximum);
        settings->setValue("RangeMinimum", mRangeMinimum);

        settings->setValue("OutOfRange", mOutOfRangeColorEnabled);
        settings->setValue("OutOfRangeColor", mOutOfRangeColorSettings.color().name(QColor::HexArgb));
        settings->setValue("OutOfRangeColorMode", mOutOfRangeColorSettings.mode());

        if(mode == cmCurveY) {
            settings->beginGroup("Plottable");
            settings->setValue("ScatterShape", mScatterShape);
            settings->setValue("ScatterSize", mScatterSize);
            settings->setValue("PenWidth", mPenWidth);

            settings->beginGroup("Curve");
            settings->setValue("LineStyle", mCurveLineStyle);
            settings->setValue("TracerVisible", mCurveTracerVisible);
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmGraph ) {
            settings->beginGroup("Plottable");
            settings->setValue("ScatterShape", mScatterShape);
            settings->setValue("ScatterSize", mScatterSize);
            settings->setValue("PenWidth", mPenWidth);

            settings->beginGroup("Graph");
            settings->setValue("Brush", mGraphBrush);
            settings->setValue("LineStyle", mGraphLineStyle);
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmFull ) {
            settings->beginGroup("Plottable");
            settings->setValue("ScatterShape", mScatterShape);
            settings->setValue("ScatterSize", mScatterSize);
            settings->setValue("PenWidth", mPenWidth);

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
                QMapIterator<double, ColorSettings> itLow(mThresholdsSettings.lowThresholds());
                while (itLow.hasNext()) {
                    itLow.next();

                    settings->setArrayIndex(lowCount);
                    settings->setValue("Color", itLow.value().color().name(QColor::HexArgb));
                    settings->setValue("Mode", itLow.value().mode());
                    settings->setValue("Value", itLow.key());
                    lowCount ++;

                }
            }
            settings->endArray();

            settings->beginWriteArray("HighMapping");
            {
                int highCount = 0;
                QMapIterator<double, ColorSettings> itHigh(mThresholdsSettings.highThresholds());
                while (itHigh.hasNext()) {
                    itHigh.next();

                    settings->setArrayIndex(highCount);
                    settings->setValue("Color", itHigh.value().color().name(QColor::HexArgb));
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

                QMapIterator<qlonglong, ColorSettings> itStateColor(mStatesSettings.statesColor());
                while (itStateColor.hasNext()) {
                    itStateColor.next();

                    settings->setArrayIndex(statesCount);
                    settings->setValue("Active", mStatesSettings.statesActive().value(itStateColor.key()));
                    settings->setValue("Color", itStateColor.value().color().name(QColor::HexArgb));
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

void ParameterConfiguration::load(QSettings *settings, ConfigurationMode mode)
{
    mLabel= settings->value("Label").toString();
    mDescription= settings->value("Description").toString();
    mUserDefinedLabel= settings->value("UserDefinedLabel").toString();
    mUserLabelEnabled= settings->value("UserLabelEnabled").toBool();
    mUserDefinedUnit= settings->value("UserDefinedUnit").toString();
    mUserUnitEnabled= settings->value("UserUnitEnabled").toBool();

    mDefaultColorSettings.setColor(QColor(settings->value("DefaultColor").toString()));
    mDefaultColorSettings.setMode(ColorSettings::ColorMode(settings->value("DefaultColorMode").toInt()));

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
        mOutOfRangeColorSettings.setMode(ColorSettings::ColorMode(settings->value("OutOfRangeColorMode").toInt()));


        if(mode == cmFull) {
            settings->beginGroup("Plottable");

            setScatterShape(QCPScatterStyle::ScatterShape(settings->value("ScatterShape").toInt()));
            setScatterSize(settings->value("ScatterSize").toInt());
            mPenWidth = settings->value("PenWidth", mPenWidth).toDouble();

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
            mPenWidth = settings->value("PenWidth", mPenWidth).toDouble();

            settings->beginGroup("Curve");
            mCurveLineStyle = QCPCurve::LineStyle(settings->value("LineStyle").toInt());
            mCurveTracerVisible = settings->value("TracerVisible").toBool();
            settings->endGroup();

            settings->endGroup();
        } else if(mode == cmGraph) {
            settings->beginGroup("Plottable");

            setScatterShape(QCPScatterStyle::ScatterShape(settings->value("ScatterShape").toInt()));
            setScatterSize(settings->value("ScatterSize").toInt());
            mPenWidth = settings->value("PenWidth", mPenWidth).toDouble();

            settings->beginGroup("Graph");
            mGraphBrush = BrushStyle(settings->value("Brush").toInt());
            mGraphLineStyle = QCPGraph::LineStyle(settings->value("LineStyle").toInt());
            settings->endGroup();

            settings->endGroup();
        }

        settings->beginGroup("Thresholds");
        {
            mThresholdsSettings.clearLowThresholds();
            int lowThresholdsCount = settings->beginReadArray("LowMapping");
            {
                for(int i=0;i<lowThresholdsCount;i++) {
                    settings->setArrayIndex(i);

                    ColorSettings cs;
                    cs.setMode(ColorSettings::ColorMode(settings->value("Mode").toInt()));
                    cs.setColor(QColor(settings->value("Color").toString()));
                    mThresholdsSettings.insertLowThreshold(settings->value("Value").toDouble(), cs);
                }
            }
            settings->endArray();

            mThresholdsSettings.clearHighThresholds();
            int highThresholdsCount = settings->beginReadArray("HighMapping");
            {
                for(int i=0;i<highThresholdsCount;i++) {
                    settings->setArrayIndex(i);

                    ColorSettings cs;
                    cs.setMode(ColorSettings::ColorMode(settings->value("Mode").toInt()));
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

                    ColorSettings cs;
                    cs.setMode(ColorSettings::ColorMode(settings->value("Mode").toInt()));
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

int ParameterConfiguration::precision() const
{
    return mPrecision;
}

void ParameterConfiguration::setPrecision(int precision)
{
    if(precision != mPrecision) {
        mPrecision = precision;
        mModified = true;
    }
}

QString ParameterConfiguration::description() const
{
    return mDescription;
}

void ParameterConfiguration::setDescription(const QString &description)
{
    if(description != mDescription) {
        mDescription = description;
        mModified = true;
    }
}

QString ParameterConfiguration::userDefinedLabel() const
{
    return mUserDefinedLabel;
}

void ParameterConfiguration::setUserDefinedLabel(const QString &userDefinedLabel)
{
    if(userDefinedLabel != mUserDefinedLabel) {
        mUserDefinedLabel = userDefinedLabel;
        mModified = true;
    }
}

QString ParameterConfiguration::userDefinedUnit() const
{
    return mUserDefinedUnit;
}

void ParameterConfiguration::setUserDefinedUnit(const QString &userDefinedUnit)
{
    if(userDefinedUnit != mUserDefinedUnit) {
        mUserDefinedUnit = userDefinedUnit;
        mModified = true;
    }
}

bool ParameterConfiguration::validRange() const
{
    return mValidRange;
}

void ParameterConfiguration::setValidRange(bool validRange)
{
    if(validRange != mValidRange) {
        mValidRange = validRange;
        mModified = true;
    }
}

double ParameterConfiguration::rangeMaximum() const
{
    return mRangeMaximum;
}

void ParameterConfiguration::setRangeMaximum(double rangeMaximum)
{
    if(!qFuzzyCompare(rangeMaximum,mRangeMaximum)) {
        mRangeMaximum = rangeMaximum;
        mModified = true;
    }
}

double ParameterConfiguration::rangeMinimum() const
{
    return mRangeMinimum;
}

void ParameterConfiguration::setRangeMinimum(double rangeMinimum)
{
    if(!qFuzzyCompare(rangeMinimum,mRangeMinimum)) {
        mRangeMinimum = rangeMinimum;
        mModified = true;
    }
}

QString ParameterConfiguration::label() const
{
    return mLabel;
}

void ParameterConfiguration::setLabel(const QString &label)
{
    if(label != mLabel) {
        mLabel = label;
        mModified = true;
    }
}

bool ParameterConfiguration::userLabelEnabled() const
{
    return mUserLabelEnabled;
}

void ParameterConfiguration::setUserLabelEnabled(bool userLabelEnabled)
{
    if(userLabelEnabled != mUserLabelEnabled) {
        mUserLabelEnabled = userLabelEnabled;
        mModified = true;
    }
}

bool ParameterConfiguration::userUnitEnabled() const
{
    return mUserUnitEnabled;
}

void ParameterConfiguration::setUserUnitEnabled(bool userUnitEnabled)
{
    if(userUnitEnabled != mUserUnitEnabled) {
        mUserUnitEnabled = userUnitEnabled;
        mModified = true;
    }
}

ColorSettings &ParameterConfiguration::defaultColorSettingsRef()
{
    return mDefaultColorSettings;
}

Thresholds &ParameterConfiguration::thresholdsSettingsRef()
{
    return mThresholdsSettings;
}

ColorSettings &ParameterConfiguration::outOfRangeColorSettingsRef()
{
    return mOutOfRangeColorSettings;
}

bool ParameterConfiguration::outOfRangeColorEnabled() const
{
    return mOutOfRangeColorEnabled;
}

void ParameterConfiguration::setOutOfRangeColorEnabled(bool outOfRangeColorEnabled)
{
    mOutOfRangeColorEnabled = outOfRangeColorEnabled;
}

States &ParameterConfiguration::statesSettingsRef()
{
    return mStatesSettings;
}

Bitfield &ParameterConfiguration::bitfieldsSettingsRef()
{
    return mBitfieldsSettings;
}

QString ParameterConfiguration::saveFilePath() const
{
    return mSaveFilePath;
}

double ParameterConfiguration::penWidth() const
{
    return mPenWidth;
}

void ParameterConfiguration::setPenWidth(double penWidth)
{
    if(!qFuzzyCompare(penWidth,mPenWidth)) {
        mPenWidth = penWidth;
        mModified = true;
    }
}

ParameterConfiguration::ItemColor ParameterConfiguration::itemColorMode() const
{
    return mItemColorMode;
}

void ParameterConfiguration::setItemColorMode(const ItemColor &itemColorMode)
{
    if(itemColorMode != mItemColorMode) {
        mItemColorMode = itemColorMode;
        mModified = true;
    }
}

QColor ParameterConfiguration::itemStaticColor() const
{
    return mItemStaticColor;
}

void ParameterConfiguration::setItemStaticColor(const QColor &itemStaticColor)
{
    if(itemStaticColor != mItemStaticColor) {
        mItemStaticColor = itemStaticColor;
        mModified = true;
    }
}

bool ParameterConfiguration::curveTracerVisible() const
{
    return mCurveTracerVisible;
}

void ParameterConfiguration::setCurveTracerVisible(bool visible)
{
    if(visible != mCurveTracerVisible) {
        mCurveTracerVisible = visible;
        mModified = true;
    }
}

bool ParameterConfiguration::modified()
{
    mModified = mModified || mThresholdsSettings.modified();
    mModified = mModified || mStatesSettings.modified();
    mModified = mModified || mBitfieldsSettings.modified();
    mModified = mModified || mOutOfRangeColorSettings.modified();
    mModified = mModified || mDefaultColorSettings.modified();
    return mModified;
}

void ParameterConfiguration::setModified(bool modified)
{
    mModified = modified;
    mThresholdsSettings.setModified(modified);
    mStatesSettings.setModified(modified);
    mBitfieldsSettings.setModified(modified);
    mOutOfRangeColorSettings.setModified(modified);
    mDefaultColorSettings.setModified(modified);
}

bool ParameterConfiguration::itemsThresholdsVisible() const
{
    return mItemsThresholdsVisible;
}

void ParameterConfiguration::setItemsThresholdsVisible(bool itemsThresholdsVisible)
{
    if(itemsThresholdsVisible != mItemsThresholdsVisible) {
        mItemsThresholdsVisible = itemsThresholdsVisible;
        mModified = true;
    }
}

ParameterConfiguration::BrushStyle ParameterConfiguration::graphBrush() const
{
    return mGraphBrush;
}

void ParameterConfiguration::setGraphBrush(BrushStyle graphBrush)
{
    if(graphBrush != mGraphBrush) {
        mGraphBrush = graphBrush;
        mModified = true;
    }
}

QCPGraph::LineStyle ParameterConfiguration::graphLineStyle() const
{
    return mGraphLineStyle;
}

void ParameterConfiguration::setGraphLineStyle(const QCPGraph::LineStyle &graphLineStyle)
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

QCPScatterStyle::ScatterShape ParameterConfiguration::scatterShape() const
{
    return mScatterShape;
}

void ParameterConfiguration::setScatterShape(const QCPScatterStyle::ScatterShape &scatterShape)
{
    if(scatterShape != mScatterShape) {
        mScatterShape = scatterShape;
        mScatterStyle.setShape(mScatterShape);
        mModified = true;
    }
}

int ParameterConfiguration::scatterSize() const
{
    return mScatterSize;
}

void ParameterConfiguration::setScatterSize(int scatterSize)
{
    if(scatterSize != mScatterSize) {
        mScatterSize = scatterSize;
        mScatterStyle.setSize(mScatterSize);
        mModified = true;
    }
}

QCPScatterStyle ParameterConfiguration::scatterStyle() const
{
    return mScatterStyle;
}

QCPCurve::LineStyle ParameterConfiguration::curveLineStyle() const
{
    return mCurveLineStyle;
}

void ParameterConfiguration::setCurveLineStyle(const QCPCurve::LineStyle &curveLineStyle)
{
    if(curveLineStyle != mCurveLineStyle) {
        mCurveLineStyle = curveLineStyle;
        mModified = true;
    }
}

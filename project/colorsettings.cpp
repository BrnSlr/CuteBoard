#include "colorsettings.h"

#include <utility>

QTBColorSettings::QTBColorSettings() :
    mLightColor(QColor(220,230,240)),
    mDarkColor(QColor(25,35,45)),
    mColor(mLightColor),
    mMode(cmAuto),
    mModified(false)
{
    updateColors();
}

void QTBColorSettings::setColor(const QColor &color)
{
    if(color != mColor) {
        mColor = color;
        updateColors();
        mModified = true;
    }
}

QColor QTBColorSettings::color() const
{
    return mColor;
}

double QTBColorSettings::luminance(const QColor& color)
{
    double red = color.redF();
    double green = color.greenF();
    double blue = color.blueF();

    double rred = (red <= 0.03928) ? red / 12.92 : qPow((red + 0.055) / 1.055, 2.4);
    double ggreen = (green <= 0.03928) ? green / 12.92 : qPow((green + 0.055) / 1.055, 2.4);
    double bblue = (blue <= 0.03928) ? blue / 12.92 : qPow((blue + 0.055) / 1.055, 2.4);

    return 0.2126 * rred + 0.7152 * ggreen + 0.0722 * bblue;
}

double QTBColorSettings::contrast(QColor rgb1, QColor rgb2)
{
    double lum1 = luminance(rgb1);
    double lum2  = luminance(rgb2);

    if(lum1 > lum2) {
        return (lum1 + 0.05) / (lum2 + 0.05);
    } 
        return (lum2 + 0.05) / (lum1 + 0.05);
    
}

QTBColorSettings::ColorMode QTBColorSettings::mode() const
{
    return mMode;
}

void QTBColorSettings::setMode(const ColorMode &mode)
{
    if(mode != mMode) {
        mMode = mode;
        updateColors();
        mModified = true;
    }
}

QColor QTBColorSettings::foregroundColor() const
{
    return mForegroundColor;
}

QBrush QTBColorSettings::backgroundBrush() const
{
    return mBackgroundBrush;
}

QColor QTBColorSettings::lightColor() const
{
    return mLightColor;
}

QColor QTBColorSettings::darkColor() const
{
    return mDarkColor;
}

void QTBColorSettings::updateColors()
{
    switch(mMode){
    case cmAuto:
    {
        QColor tmpColor = (contrast(mColor, mLightColor) > contrast(mColor, mDarkColor)) ? mLightColor : mDarkColor;
        if(tmpColor == mDarkColor) {
            mForegroundColor = mColor;
            mBackgroundBrush = Qt::NoBrush;
        } else {
            mForegroundColor = tmpColor;
            mBackgroundBrush = QBrush(mColor);
        }
    }
        break;
    case cmFilledBackground:
        mForegroundColor = (contrast(mColor, mLightColor) > contrast(mColor, mDarkColor)) ? mLightColor : mDarkColor;
        mBackgroundBrush = QBrush(mColor);
        break;
    case cmNoBackground:
        mForegroundColor = mColor;
        mBackgroundBrush = Qt::NoBrush;
        break;
    }
}

bool QTBColorSettings::modified() const
{
    return mModified;
}

void QTBColorSettings::setModified(bool modified)
{
    mModified = modified;
}

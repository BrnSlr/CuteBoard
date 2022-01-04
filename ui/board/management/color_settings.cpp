#include "color_settings.h"

#include <utility>

ColorSettings::ColorSettings(QColor lightColor,
                             QColor darkColor) :
    mLightColor(lightColor),
    mDarkColor(darkColor),
    mColor(mLightColor),
    mMode(cmFilledBackground),
    mModified(false)
{
    updateColors();
}

void ColorSettings::setColor(const QColor &color)
{
    if(color != mColor) {
        mColor = color;
        updateColors();
        mModified = true;
    }
}

QColor ColorSettings::color() const
{
    return mColor;
}

double ColorSettings::luminance(const QColor& color)
{
    double red = color.redF();
    double green = color.greenF();
    double blue = color.blueF();

    double rred = (red <= 0.03928) ? red / 12.92 : qPow((red + 0.055) / 1.055, 2.4);
    double ggreen = (green <= 0.03928) ? green / 12.92 : qPow((green + 0.055) / 1.055, 2.4);
    double bblue = (blue <= 0.03928) ? blue / 12.92 : qPow((blue + 0.055) / 1.055, 2.4);

    return 0.2126 * rred + 0.7152 * ggreen + 0.0722 * bblue;
}

double ColorSettings::contrast(QColor rgb1, QColor rgb2)
{
    double lum1 = luminance(rgb1);
    double lum2  = luminance(rgb2);

    if(lum1 > lum2) {
        return (lum1 + 0.05) / (lum2 + 0.05);
    }
    return (lum2 + 0.05) / (lum1 + 0.05);
    
}

ColorSettings::ColorMode ColorSettings::mode() const
{
    return mMode;
}

void ColorSettings::setMode(const ColorMode &mode)
{
    if(mode != mMode) {
        mMode = mode;
        updateColors();
        mModified = true;
    }
}

QColor ColorSettings::foregroundColor() const
{
    return mForegroundColor;
}

QBrush ColorSettings::backgroundBrush() const
{
    return mBackgroundBrush;
}

QColor ColorSettings::lightColor() const
{
    return mLightColor;
}

QColor ColorSettings::darkColor() const
{
    return mDarkColor;
}

void ColorSettings::updateColors()
{
    switch(mMode){
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

bool ColorSettings::modified() const
{
    return mModified;
}

void ColorSettings::setModified(bool modified)
{
    mModified = modified;
}

void ColorSettings::setLightColor(const QColor &lightColor)
{
    mLightColor = lightColor;
}

void ColorSettings::setDarkColor(const QColor &darkColor)
{
    mDarkColor = darkColor;
}

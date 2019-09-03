#ifndef COLORSETTINGS_H
#define COLORSETTINGS_H

#include <QColor>
#include <QBrush>
#include <QDebug>
#include <qmath.h>

class QTBColorSettings
{
public:
    enum ColorMode {
        cmAuto,
        cmFilledBackground,
        cmNoBackground
    };
    QTBColorSettings();

    void setColor(const QColor &color);
    QColor color() const;

    double luminance(const QColor& color);
    double contrast(QColor rgb1, QColor rgb2);

    ColorMode mode() const;
    void setMode(const ColorMode &mode);

    QColor foregroundColor() const;
    QBrush backgroundBrush() const;

    QColor lightColor() const;
    QColor darkColor() const;

    bool modified() const;
    void setModified(bool modified);

protected:
    QColor mLightColor;
    QColor mDarkColor;

    QColor mColor;
    QColor mForegroundColor;
    QBrush mBackgroundBrush;

    ColorMode mMode;

    void updateColors();
    bool mModified;
};

#endif // COLORSETTINGS_H

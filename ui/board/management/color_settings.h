#ifndef COLORSETTINGS_H
#define COLORSETTINGS_H

#include <QColor>
#include <QBrush>
#include <QDebug>
#include <qmath.h>
#include "ui/ui_global.h"

class BOARD_UI_EXPORT ColorSettings
{
public:
    enum ColorMode {
        cmFilledBackground = 1,
        cmNoBackground = 2
    };
    ColorSettings(QColor lightColor = QColor(Qt::white),
                  QColor darkColor = QColor(Qt::black) /*QColor(41,56,67)*/);

    void setColor(const QColor &color);
    QColor color() const;

    static double luminance(const QColor& color);
    static double contrast(QColor rgb1, QColor rgb2);

    ColorMode mode() const;
    void setMode(const ColorMode &mode);

    QColor foregroundColor() const;
    QBrush backgroundBrush() const;

    QColor lightColor() const;
    QColor darkColor() const;

    bool modified() const;
    void setModified(bool modified);

    void setLightColor(const QColor &lightColor);
    void setDarkColor(const QColor &darkColor);

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

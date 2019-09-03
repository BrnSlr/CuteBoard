#ifndef COLORLINEEDIT_H
#define COLORLINEEDIT_H

#include <QLineEdit>
#include <QColor>
#include "project/colorsettings.h"

class ColorLineEdit: public QLineEdit
{
    Q_OBJECT
public:
    ColorLineEdit(QWidget* parent = nullptr);
    ~ColorLineEdit() Q_DECL_OVERRIDE;

    void setMode(const QTBColorSettings::ColorMode &mode);
    void setColor(const QColor& color);
    QColor color() const;
    QTBColorSettings::ColorMode mode() const;

    bool colorText() const;
    void setColorText(bool colorText);

protected:
    bool mColorText;
    QTBColorSettings *mColorSettings;
    QColor colorFromString(const QString& string);
};

#endif // COLORLINEEDIT_H

#include "colorlineedit.h"
#include <QPainter>
#include <QStyleOptionFrame>

static QRegularExpression regex_qcolor (QStringLiteral("^(?:(?:#[[:xdigit:]]{3})|(?:#[[:xdigit:]]{6})|(?:[[:alpha:]]+))$"));
static QRegularExpression regex_func_rgb (QStringLiteral(R"(^rgb\s*\(\s*([0-9]+)\s*,\s*([0-9]+)\s*,\s*([0-9]+)\s*\)$)"));

ColorLineEdit::ColorLineEdit(QWidget *parent) : QLineEdit (parent),
    mColorText(true),
    mColorSettings(new QTBColorSettings)
{
    setColor(mColorSettings->color());
}

ColorLineEdit::~ColorLineEdit()
{

}

void ColorLineEdit::setMode(const QTBColorSettings::ColorMode &mode)
{
    mColorSettings->setMode(mode);
    if(mColorSettings->backgroundBrush() == Qt::NoBrush) {
        setStyleSheet(
                    QString("background-color: %1; color: %2;")
                    .arg(mColorSettings->darkColor().name())
                    .arg(mColorSettings->foregroundColor().name())
                    );
    } else {
        setStyleSheet(
                    QString("background-color: %1; color: %2;")
                    .arg(mColorSettings->backgroundBrush().color().name())
                    .arg(mColorSettings->foregroundColor().name())
                    );
    }
}

void ColorLineEdit::setColor(const QColor &color)
{
    mColorSettings->setColor(color);

    if(mColorSettings->backgroundBrush() == Qt::NoBrush) {
        setStyleSheet(
                    QString("background-color: %1; color: %2;")
                    .arg(mColorSettings->darkColor().name())
                    .arg(mColorSettings->foregroundColor().name())
                    );
    } else {
        setStyleSheet(
                    QString("background-color: %1; color: %2;")
                    .arg(mColorSettings->backgroundBrush().color().name())
                    .arg(mColorSettings->foregroundColor().name())
                    );
    }
    if(mColorText)
        setText(color.name());
}

QColor ColorLineEdit::color() const
{
    return mColorSettings->color();
}

QTBColorSettings::ColorMode ColorLineEdit::mode() const
{
    return mColorSettings->mode();
}

bool ColorLineEdit::colorText() const
{
    return mColorText;
}

void ColorLineEdit::setColorText(bool colorText)
{
    mColorText = colorText;
}

QColor ColorLineEdit::colorFromString(const QString &string)
{
    QString xs = string.trimmed();
    QRegularExpressionMatch match;

    match = regex_qcolor.match(xs);
    if ( match.hasMatch() )
    {
        return QColor(xs);
    }

    match = regex_func_rgb.match(xs);
    if ( match.hasMatch() )
    {
        return QColor(
                    match.captured(1).toInt(),
                    match.captured(2).toInt(),
                    match.captured(3).toInt()
                    );
    }
    return QColor();

}

#include "color_line_edit.h"
#include <QPainter>
#include <QStyleOptionFrame>

static QRegularExpression regex_qcolor (QStringLiteral("^(?:(?:#[[:xdigit:]]{3})|(?:#[[:xdigit:]]{6})|(?:[[:alpha:]]+))$"));
static QRegularExpression regex_func_rgb (QStringLiteral(R"(^rgb\s*\(\s*([0-9]+)\s*,\s*([0-9]+)\s*,\s*([0-9]+)\s*\)$)"));

ColorLineEdit::ColorLineEdit(QWidget *parent) : QLineEdit (parent),
    mColorText(true),
    mColorSettings(new ColorSettings)
{
    setColor(mColorSettings->color());

    installEventFilter(this);
}

ColorLineEdit::~ColorLineEdit()
{

}

void ColorLineEdit::setMode(const ColorSettings::ColorMode &mode)
{
    mColorSettings->setMode(mode);
    if(mColorSettings->backgroundBrush() == Qt::NoBrush) {
        setStyleSheet(
                    QString("color: %1;")
                    .arg(mColorSettings->foregroundColor().name(QColor::HexArgb))
                    );
    } else {
        setStyleSheet(
                    QString("background-color: %1; color: %2;")
                    .arg(mColorSettings->backgroundBrush().color().name(QColor::HexArgb))
                    .arg(mColorSettings->foregroundColor().name(QColor::HexArgb))
                    );
    }
}

void ColorLineEdit::setColor(const QColor &color)
{
    mColorSettings->setColor(color);

    if(mColorSettings->backgroundBrush() == Qt::NoBrush) {
        setStyleSheet(
                    QString("color: %1;")
                    .arg(mColorSettings->foregroundColor().name(QColor::HexArgb))
                    );
    } else {
        setStyleSheet(
                    QString("background-color: %1; color: %2;")
                    .arg(mColorSettings->backgroundBrush().color().name(QColor::HexArgb))
                    .arg(mColorSettings->foregroundColor().name(QColor::HexArgb))
                    );
    }
    if(mColorText)
        setText(color.name(QColor::HexArgb));
}

QColor ColorLineEdit::color() const
{
    return mColorSettings->color();
}

ColorSettings::ColorMode ColorLineEdit::mode() const
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

bool ColorLineEdit::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)
    if(ev->type() == QEvent::MouseButtonPress) {
        emit clicked();
    }
    return false;
}

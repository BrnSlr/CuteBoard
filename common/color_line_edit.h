#ifndef COLORLINEEDIT_H
#define COLORLINEEDIT_H

#include <QLineEdit>
#include <QColor>
#include <QEvent>
#include "ui/board/management/color_settings.h"
#include "ui/ui_global.h"
#include "log4qt/logger.h"

class BOARD_UI_EXPORT ColorLineEdit: public QLineEdit
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    ColorLineEdit(QWidget* parent = nullptr);
    ~ColorLineEdit() Q_DECL_OVERRIDE;

    void setMode(const ColorSettings::ColorMode &mode);
    void setColor(const QColor& color);
    QColor color() const;
    ColorSettings::ColorMode mode() const;

    bool colorText() const;
    void setColorText(bool colorText);

signals:
    void clicked();

protected:
    QColor colorFromString(const QString& string);
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    bool mColorText;
    ColorSettings *mColorSettings;
};

#endif // COLORLINEEDIT_H

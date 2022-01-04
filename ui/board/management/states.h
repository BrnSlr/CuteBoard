#ifndef STATESSETTINGS_H
#define STATESSETTINGS_H

#include "color_settings.h"
#include "ui/ui_global.h"

class BOARD_UI_EXPORT States
{
public:
    States();
    ~States();

    void clearStates();
    void addState(bool active, qlonglong value, const QString& text, const ColorSettings&);

    QString text(qlonglong value);
    ColorSettings colorSettings(qlonglong value, bool &colorIsSet);

    bool active();

    QMap<qlonglong, ColorSettings> statesColor() const;
    QMap<qlonglong, QString> statesText() const;
    QMap<qlonglong, bool> statesActive() const;

    bool modified() const;
    void setModified(bool modified);


protected:
    QMap<qlonglong, ColorSettings> mStatesColor;
    QMap<qlonglong, QString> mStatesText;
    QMap<qlonglong, bool> mStatesActive;
    bool mModified;
};

#endif // STATESSETTINGS_H

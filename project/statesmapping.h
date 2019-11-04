#ifndef STATESSETTINGS_H
#define STATESSETTINGS_H

#include "colorsettings.h"

class QTBStatesMapping
{
public:
    QTBStatesMapping();
    ~QTBStatesMapping();

    void clearStates();
    void addState(bool active, qlonglong value, const QString& text, const QTBColorSettings&);

    QString text(qlonglong value);
    QTBColorSettings colorSettings(qlonglong value, bool &colorIsSet);

    bool active();

    QMap<qlonglong, QTBColorSettings> statesColor() const;
    QMap<qlonglong, QString> statesText() const;
    QMap<qlonglong, bool> statesActive() const;

    bool modified() const;
    void setModified(bool modified);


protected:
    QMap<qlonglong, QTBColorSettings> mStatesColor;
    QMap<qlonglong, QString> mStatesText;
    QMap<qlonglong, bool> mStatesActive;
    bool mModified;
};

#endif // STATESSETTINGS_H

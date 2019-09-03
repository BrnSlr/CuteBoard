#ifndef STATESSETTINGS_H
#define STATESSETTINGS_H

#include "colorsettings.h"

class QTBStatesMapping
{
public:
    QTBStatesMapping();
    ~QTBStatesMapping();

    void clearStates();
    void addState(qlonglong value, const QString& text, const QTBColorSettings&);

    QString text(qlonglong value);
    QTBColorSettings colorSettings(qlonglong value, bool &colorIsSet);

    bool active();

    QMap<qlonglong, QTBColorSettings> statesColor() const;
    QMap<qlonglong, QString> statesText() const;

    bool modified() const;
    void setModified(bool modified);

protected:
    QMap<qlonglong, QTBColorSettings> mStatesColor;
    QMap<qlonglong, QString> mStatesText;
    bool mModified;
};

#endif // STATESSETTINGS_H

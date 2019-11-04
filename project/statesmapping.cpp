#include "statesmapping.h"

QTBStatesMapping::QTBStatesMapping():
    mModified(false)
{

}

QTBStatesMapping::~QTBStatesMapping()
{
    mStatesText.clear();
    mStatesColor.clear();
    mStatesActive.clear();
}

void QTBStatesMapping::addState(bool active,
                                qlonglong value,
                                const QString& text,
                                const QTBColorSettings& colorSettings)
{
    mStatesActive.remove(value);
    mStatesActive.insert(value, active);

    mStatesColor.remove(value);
    mStatesColor.insert(value, colorSettings);

    mStatesText.remove(value);
    mStatesText.insert(value, text);
    mModified = true;
}

QString QTBStatesMapping::text(qlonglong value)
{

    if(mStatesActive.contains(value)) {
        if(mStatesActive.value(value)) {
            return mStatesText.value(value);
        }
    }
    return QString();
}

QTBColorSettings QTBStatesMapping::colorSettings(qlonglong value, bool &colorIsSet)
{
    QTBColorSettings cs;
    colorIsSet = false;
    if(mStatesActive.contains(value)) {
        if(mStatesActive.value(value)) {
            cs = mStatesColor.value(value);
            colorIsSet = true;
        }
    }
    return cs;
}

bool QTBStatesMapping::active()
{
    if(mStatesColor.count() > 0)
        return true;
    return false;
}

void QTBStatesMapping::clearStates()
{
    mStatesColor.clear();
    mStatesText.clear();
    mStatesActive.clear();
    mModified = true;
}

QMap<qlonglong, QTBColorSettings> QTBStatesMapping::statesColor() const
{
    return mStatesColor;
}

QMap<qlonglong, QString> QTBStatesMapping::statesText() const
{
    return mStatesText;
}

bool QTBStatesMapping::modified() const
{
    return mModified;
}

void QTBStatesMapping::setModified(bool modified)
{
    mModified = modified;
}

QMap<qlonglong, bool> QTBStatesMapping::statesActive() const
{
    return mStatesActive;
}

#include "states.h"

States::States():
    mModified(false)
{

}

States::~States()
{
    mStatesText.clear();
    mStatesColor.clear();
    mStatesActive.clear();
}

void States::addState(bool active,
                                qlonglong value,
                                const QString& text,
                                const ColorSettings& colorSettings)
{
    mStatesActive.remove(value);
    mStatesActive.insert(value, active);

    mStatesColor.remove(value);
    mStatesColor.insert(value, colorSettings);

    mStatesText.remove(value);
    mStatesText.insert(value, text);
    mModified = true;
}

QString States::text(qlonglong value)
{
    if(mStatesActive.contains(value)) {
        if(mStatesActive.value(value)) {
            return mStatesText.value(value);
        }
    }
    return QString();
}

ColorSettings States::colorSettings(qlonglong value, bool &colorIsSet)
{
    ColorSettings cs;
    colorIsSet = false;
    if(mStatesActive.contains(value)) {
        if(mStatesActive.value(value)) {
            cs = mStatesColor.value(value);
            colorIsSet = true;
        }
    }
    return cs;
}

bool States::active()
{
    if(mStatesColor.count() > 0)
        return true;
    return false;
}

void States::clearStates()
{
    mStatesColor.clear();
    mStatesText.clear();
    mStatesActive.clear();
    mModified = true;
}

QMap<qlonglong, ColorSettings> States::statesColor() const
{
    return mStatesColor;
}

QMap<qlonglong, QString> States::statesText() const
{
    return mStatesText;
}

bool States::modified() const
{
    return mModified;
}

void States::setModified(bool modified)
{
    mModified = modified;
}

QMap<qlonglong, bool> States::statesActive() const
{
    return mStatesActive;
}

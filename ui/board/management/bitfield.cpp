#include "bitfield.h"

#include <utility>

Bitfield::Bitfield() :
    mModified(false)
{
    for(int i=0; i< 32; i++) {
        mBitLogics.append(true);
        mBitDescriptions.append(QString("#Bit %1").arg(i, 2, 10, QLatin1Char('0')));
    }
}

Bitfield::~Bitfield()
{
    mBitDescriptions.clear();
    mBitLogics.clear();
}

void Bitfield::setBitLogic(int bitNumber, bool oneLogic)
{
    if(bitNumber < 32) {
        mBitLogics[bitNumber] = oneLogic;
        mModified = true;
    }
}

void Bitfield::setBitDescription(int bitNumber, QString description)
{
    if(bitNumber < 32) {
        mBitDescriptions[bitNumber]=std::move(description);
        mModified = true;
    }
}

bool Bitfield::modified() const
{
    return mModified;
}

void Bitfield::setModified(bool modified)
{
    mModified = modified;
}

QList<bool> &Bitfield::bitLogics()
{
    return mBitLogics;
}

QList<QString> &Bitfield::bitDescriptions()
{
    return mBitDescriptions;
}

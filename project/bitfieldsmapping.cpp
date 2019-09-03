#include "bitfieldsmapping.h"

#include <utility>

QTBBitfieldsMapping::QTBBitfieldsMapping() :
    mModified(false)
{
    for(int i=0; i< 32; i++) {
        mBitLogics.append(true);
        mBitDescriptions.append(QString());
    }
}

QTBBitfieldsMapping::~QTBBitfieldsMapping()
{
    mBitDescriptions.clear();
    mBitLogics.clear();
}

void QTBBitfieldsMapping::setBitLogic(int bitNumber, bool oneLogic)
{
    mBitLogics[bitNumber] = oneLogic;
    mModified = true;
}

void QTBBitfieldsMapping::setBitDescription(int bitNumber, QString description)
{
    mBitDescriptions[bitNumber]=std::move(description);
    mModified = true;
}

bool QTBBitfieldsMapping::modified() const
{
    return mModified;
}

void QTBBitfieldsMapping::setModified(bool modified)
{
    mModified = modified;
}

QList<bool> &QTBBitfieldsMapping::bitLogics()
{
    return mBitLogics;
}

QList<QString> &QTBBitfieldsMapping::bitDescriptions()
{
    return mBitDescriptions;
}

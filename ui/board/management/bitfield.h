#ifndef BITFIELDSMAPPING_H
#define BITFIELDSMAPPING_H

#include <QMap>
#include "ui/ui_global.h"

class BOARD_UI_EXPORT Bitfield
{
public:
    Bitfield();
    ~Bitfield();

    void setBitLogic(int bitNumber, bool oneLogic);
    void setBitDescription(int bitNumber, QString description);

    bool modified() const;
    void setModified(bool modified);

    QList<bool>& bitLogics();
    QList<QString>& bitDescriptions();

protected:
    QList<bool> mBitLogics;
    QList<QString> mBitDescriptions;

    bool mModified;
};

#endif // BITFIELDSMAPPING_H

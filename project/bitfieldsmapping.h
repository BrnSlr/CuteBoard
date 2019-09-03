#ifndef BITFIELDSMAPPING_H
#define BITFIELDSMAPPING_H

#include <QMap>

class QTBBitfieldsMapping
{
public:
    QTBBitfieldsMapping();
    ~QTBBitfieldsMapping();

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

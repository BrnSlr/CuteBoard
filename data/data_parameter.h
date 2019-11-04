#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>

class QTBParameter
{
public:
    explicit QTBParameter();
    QTBParameter(const QTBParameter *parameter);

    quint32 parameterId() const;

    QString label() const;
    void setLabel(const QString &label);

    QString unit() const;
    void setUnit(const QString &unit);  

    QString sourceName() const;
    void setSourceName(const QString &sourceName);

protected:
    quint32         mParameterId;
    QString         mLabel;
    QString         mUnit;
    QString         mSourceName;

    void setParameterId(const quint32 &parameterId);

    friend class QTBDataManager;
};

#endif // PARAMETER_H

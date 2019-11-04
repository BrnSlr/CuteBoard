#include "data_parameter.h"

QTBParameter::QTBParameter() :
    mParameterId(0)
{

}

QTBParameter::QTBParameter(const QTBParameter *parameter) :
    mParameterId(parameter->parameterId()),
    mLabel(parameter->label()),
    mUnit(parameter->unit())
{

}

quint32 QTBParameter::parameterId() const
{
    return mParameterId;
}

void QTBParameter::setParameterId(const quint32 &parameterId)
{
    mParameterId = parameterId;
}

QString QTBParameter::label() const
{
    return mLabel;
}

void QTBParameter::setLabel(const QString &label)
{
    mLabel = label;
}

QString QTBParameter::unit() const
{
    return mUnit;
}

void QTBParameter::setUnit(const QString &unit)
{
    mUnit = unit;
}

QString QTBParameter::sourceName() const
{
    return mSourceName;
}

void QTBParameter::setSourceName(const QString &sourceName)
{
    mSourceName = sourceName;
}

//void QTBParameter::setData(const QSharedPointer<QTBData> &data)
//{
//    mData = data;
//}

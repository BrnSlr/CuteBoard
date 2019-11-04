#ifndef DATAVALUE_H
#define DATAVALUE_H

#include <QtGlobal>
#include <QString>
#include <cstdlib>
#include <cmath>

/* Classe de gestion des differents types informatiques*/
struct s_int8 {
    qint8   int8;
    qint8   padding3;
    qint8   padding2;
    qint8   padding1;
};
Q_DECLARE_TYPEINFO(s_int8,      Q_PRIMITIVE_TYPE);

struct s_uint8 {
    quint8  uint8;
    quint8  padding3;
    quint8  padding2;
    quint8  padding1;
};
Q_DECLARE_TYPEINFO(s_uint8,     Q_PRIMITIVE_TYPE);

struct s_int16 {
    qint16  int16;
    qint16  padding1;
};
Q_DECLARE_TYPEINFO(s_int16,     Q_PRIMITIVE_TYPE);

struct s_uint16 {
    quint16     uint16;
    quint16      padding1;
};
Q_DECLARE_TYPEINFO(s_uint16,    Q_PRIMITIVE_TYPE);

struct s_int32 {
    qint32  int32;
    qint32  padding1;
};
Q_DECLARE_TYPEINFO(s_int32,     Q_PRIMITIVE_TYPE);

struct s_uint32 {
    quint32 uint32;
    qint32  padding1;
};
Q_DECLARE_TYPEINFO(s_uint32,    Q_PRIMITIVE_TYPE);

struct s_float {
    float   f;
    float   padding1;
};
Q_DECLARE_TYPEINFO(s_float,     Q_PRIMITIVE_TYPE);

union u_data {
    s_int8      s_i8;
    s_uint8     s_ui8;
    s_int16     s_i16;
    s_uint16    s_ui16;
    qint32      i32;
    quint32     ui32;
    float       f;
};

class QTBDataValue
{

public:
    enum ValueType
    {
        TYPE_INT8,
        TYPE_UINT8,
        TYPE_INT16,
        TYPE_UINT16,
        TYPE_INT32,
        TYPE_UINT32,
        TYPE_FLOAT
    };

    QTBDataValue ()                                {mValue.ui32  = 0;    mType=TYPE_UINT32;     }
    QTBDataValue (qint8   v)    {mValue.ui32  = 0;    mValue.s_i8.int8      = v;    mType=TYPE_INT8;       }
    QTBDataValue (quint8  v)    {mValue.ui32  = 0;    mValue.s_ui8.uint8    = v;    mType=TYPE_UINT8;      }
    QTBDataValue (qint16  v)    {mValue.ui32  = 0;    mValue.s_i16.int16    = v;    mType=TYPE_INT16;      }
    QTBDataValue (quint16 v)    {mValue.ui32  = 0;    mValue.s_ui16.uint16  = v;    mType=TYPE_UINT16;     }
    QTBDataValue (qint32  v)                       {mValue.i32   = v;    mType=TYPE_INT32;      }
    QTBDataValue (quint32 v)                       {mValue.ui32  = v;    mType=TYPE_UINT32;     }
    QTBDataValue (float   v)                       {mValue.f     = v;    mType=TYPE_FLOAT;      }

    qint8       int8_value      () {return mValue.s_i8.int8;       }
    quint8      uint8_value     () {return mValue.s_ui8.uint8;     }
    qint16      int16_value     () {return mValue.s_i16.int16;     }
    quint16     uint16_value    () {return mValue.s_ui16.uint16;   }
    qint32      int32_value     () {return mValue.i32;    }
    quint32     uint32_value    () {return mValue.ui32;   }
    float       float_value     () {return mValue.f;      }

    QString toString(int precision) {
        switch(mType) {
        case TYPE_FLOAT:
            return QString::number(double(mValue.f), 'f', precision);
        case TYPE_INT8:
            return QString::number(mValue.s_i8.int8);
        case TYPE_UINT8:
            return QString::number(mValue.s_ui8.uint8);
        case TYPE_INT16:
            return QString::number(mValue.s_i16.int16);
        case TYPE_UINT16:
            return QString::number(mValue.s_ui16.uint16);
        case TYPE_INT32:
            return QString::number(mValue.i32);
        case TYPE_UINT32:
            return QString::number(mValue.ui32);
        }
        return QString();
    }

    double toDouble() {
        switch(mType) {
        case TYPE_FLOAT:
            return double(mValue.f);
        case TYPE_INT8:
            return double(mValue.s_i8.int8);
        case TYPE_UINT8:
            return double(mValue.s_ui8.uint8);
        case TYPE_INT16:
            return double(mValue.s_i16.int16);
        case TYPE_UINT16:
            return double(mValue.s_ui16.uint16);
        case TYPE_INT32:
            return double(mValue.i32);
        case TYPE_UINT32:
            return double(mValue.ui32);
        }
        return qQNaN();
    }

    u_data      mValue;
    ValueType   mType;
};

#endif // DATAVALUE_H

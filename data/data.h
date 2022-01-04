#ifndef DATAVALUE_H
#define DATAVALUE_H

#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <cstdlib>
#include <cmath>
#include <bitset>
#include "data_global.h"

#define DEFAULT_TS_SIZE 400

/* Classe de gestion des differents types informatiques*/
struct s_int8 {
    qint8   int8;
    qint8   padding4;
    qint8   padding3;
    qint8   padding2;
    qint32  padding1;
};
Q_DECLARE_TYPEINFO(s_int8,      Q_PRIMITIVE_TYPE);

struct s_uint8 {
    quint8  uint8;
    quint8  padding4;
    quint8  padding3;
    quint8  padding2;
    qint32  padding1;
};
Q_DECLARE_TYPEINFO(s_uint8,     Q_PRIMITIVE_TYPE);

struct s_int16 {
    qint16  int16;
    qint16  padding2;
    qint32  padding1;
};
Q_DECLARE_TYPEINFO(s_int16,     Q_PRIMITIVE_TYPE);

struct s_uint16 {
    quint16     uint16;
    quint16     padding2;
    qint32      padding1;
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
    s_int32     s_i32;
    s_uint32    s_ui32;
    s_float     s_f;
    qint64      i64;
    quint64     ui64;
    double      d;
};

class DATASHARED_EXPORT DataValue
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
        TYPE_INT64,
        TYPE_UINT64,
        TYPE_FLOAT,
        TYPE_DOUBLE
    };

    DataValue (qint8   v)    {mValue.ui64  = 0;    mValue.s_i8.int8      = v;    mType=TYPE_INT8;       }
    DataValue (quint8  v)    {mValue.ui64  = 0;    mValue.s_ui8.uint8    = v;    mType=TYPE_UINT8;      }
    DataValue (qint16  v)    {mValue.ui64  = 0;    mValue.s_i16.int16    = v;    mType=TYPE_INT16;      }
    DataValue (quint16 v)    {mValue.ui64  = 0;    mValue.s_ui16.uint16  = v;    mType=TYPE_UINT16;     }
    DataValue (qint32  v)    {mValue.ui64  = 0;    mValue.s_i32.int32    = v;    mType=TYPE_INT32;      }
    DataValue (quint32 v)    {mValue.ui64  = 0;    mValue.s_ui32.uint32  = v;    mType=TYPE_UINT32;     }
    DataValue (float   v)    {mValue.ui64  = 0;    mValue.s_f.f          = v;    mType=TYPE_FLOAT;      }
    DataValue (qint64  v)    {mValue.i64   = v;    mType=TYPE_INT64;      }
    DataValue (quint64 v)    {mValue.ui64  = v;    mType=TYPE_UINT64;     }
    DataValue (double  v)    {mValue.d     = v;    mType=TYPE_DOUBLE;     }
    DataValue ()             {mValue.ui64  = 0;    mType=TYPE_UINT32;     }

    qint8       int8_value      () const { return mValue.s_i8.int8;       }
    quint8      uint8_value     () const { return mValue.s_ui8.uint8;     }
    qint16      int16_value     () const { return mValue.s_i16.int16;     }
    quint16     uint16_value    () const { return mValue.s_ui16.uint16;   }
    qint32      int32_value     () const { return mValue.s_i32.int32;     }
    quint32     uint32_value    () const { return mValue.s_ui32.uint32;   }
    qint64      int64_value     () const { return mValue.i64;             }
    quint64     uint64_value    () const { return mValue.ui64;            }
    float       float_value     () const { return mValue.s_f.f;           }
    double      double_value    () const { return mValue.d;               }

    void int8_value   (qint8   v)      {mValue.s_i8.int8      = v;    mType=ValueType::TYPE_INT8;     }
    void uint8_value  (quint8  v)      {mValue.s_ui8.uint8    = v;    mType=ValueType::TYPE_UINT8;    }
    void int16_value  (qint16  v)      {mValue.s_i16.int16    = v;    mType=ValueType::TYPE_INT16;    }
    void uint16_value (quint16 v)      {mValue.s_ui16.uint16  = v;    mType=ValueType::TYPE_UINT16;   }
    void int32_value  (qint32  v)      {mValue.s_i32.int32    = v;    mType=ValueType::TYPE_INT32;    }
    void uint32_value (quint32 v)      {mValue.s_ui32.uint32  = v;    mType=ValueType::TYPE_UINT32;   }
    void int64_value  (qint64  v)      {mValue.i64            = v;    mType=ValueType::TYPE_INT64;    }
    void uint64_value (quint64 v)      {mValue.ui64           = v;    mType=ValueType::TYPE_UINT64;   }
    void float_value  (float   v)      {mValue.s_f.f          = v;    mType=ValueType::TYPE_FLOAT;    }
    void double_value (double  v)      {mValue.d              = v;    mType=ValueType::TYPE_DOUBLE;   }


    QString toString(int precision = 2) const {
        bool fullDouble = false;
        if(precision < 0 )
            fullDouble = true;
        switch(mType) {
        case TYPE_FLOAT:
            if(fullDouble)
                return QString("%1").arg(double(mValue.s_f.f), 0, 'f');
            else
                return QString::number(double(mValue.s_f.f), 'f', precision);
        case TYPE_DOUBLE:
            if(fullDouble)
                return QString("%1").arg(mValue.d, 0, 'f');
            else
                return QString::number(mValue.d, 'f', precision);
        case TYPE_INT8:
            return QString::number(mValue.s_i8.int8);
        case TYPE_UINT8:
            return QString::number(mValue.s_ui8.uint8);
        case TYPE_INT16:
            return QString::number(mValue.s_i16.int16);
        case TYPE_UINT16:
            return QString::number(mValue.s_ui16.uint16);
        case TYPE_INT32:
            return QString::number(mValue.s_i32.int32);
        case TYPE_UINT32:
            return QString::number(mValue.s_ui32.uint32);
        case TYPE_INT64:
            return QString::number(mValue.i64);
        case TYPE_UINT64:
            return QString::number(mValue.ui64);
        }
        return QString();
    }

    QString toHex() const {
        switch(mType) {
        case TYPE_INT8:
        case TYPE_UINT8:
            return QString("0x") + QString("%1").arg(uint8_value() , 2, 16, QChar('0')).toUpper();
        case TYPE_INT16:
        case TYPE_UINT16:
            return QString("0x") + QString("%1").arg(uint16_value() , 4, 16, QChar('0')).toUpper();
        case TYPE_FLOAT:
        case TYPE_INT32:
        case TYPE_UINT32:
            return QString("0x") + QString("%1").arg(uint32_value() , 8, 16, QChar('0')).toUpper();
        case TYPE_DOUBLE:
        case TYPE_INT64:
        case TYPE_UINT64:
            return QString("0x") + QString("%1").arg(uint64_value() , 16, 16, QChar('0')).toUpper();
        }
        return QString();
    }

    QString toBin() const {
        switch(mType) {
        case TYPE_INT8:
        case TYPE_UINT8:
            return QString("0b") + QString("%1").arg(uint8_value() , 8, 2, QChar('0'));
        case TYPE_INT16:
        case TYPE_UINT16:
            return QString("0b") + QString("%1").arg(uint16_value() , 16, 2, QChar('0'));
        case TYPE_FLOAT:
        case TYPE_INT32:
        case TYPE_UINT32:
            return QString("0b") + QString("%1").arg(uint32_value() , 32, 2, QChar('0'));
        case TYPE_DOUBLE:
        case TYPE_INT64:
        case TYPE_UINT64:
            return QString("0b") + QString("%1").arg(uint64_value() , 64, 2, QChar('0'));
        }
        return QString();
    }

    QString toTime() const {
        switch(mType) {
        case TYPE_FLOAT:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_f.f).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_DOUBLE:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.d).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_INT8:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_i8.int8).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_UINT8:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_ui8.uint8).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_INT16:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_i16.int16).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_UINT16:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_ui16.uint16).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_INT32:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_i32.int32).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_UINT32:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.s_ui32.uint32).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_INT64:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.i64).toUTC().toString("hh:mm:ss.zzz");
        case TYPE_UINT64:
            return QDateTime::fromMSecsSinceEpoch(1000*mValue.ui64).toUTC().toString("hh:mm:ss.zzz");
        }
        return QString("--:--:--.---");
    }

    double toDouble() const {
        switch(mType) {
        case TYPE_FLOAT:
            return double(mValue.s_f.f);
        case TYPE_DOUBLE:
            return mValue.d;
        case TYPE_INT8:
            return double(mValue.s_i8.int8);
        case TYPE_UINT8:
            return double(mValue.s_ui8.uint8);
        case TYPE_INT16:
            return double(mValue.s_i16.int16);
        case TYPE_UINT16:
            return double(mValue.s_ui16.uint16);
        case TYPE_INT32:
            return double(mValue.s_i32.int32);
        case TYPE_UINT32:
            return double(mValue.s_ui32.uint32);
        case TYPE_INT64:
            return double(mValue.i64);
        case TYPE_UINT64:
            return double(mValue.ui64);
        }
        return qQNaN();
    }

    quint32 toUint32() const {
        switch(mType) {
        case TYPE_FLOAT:
            return quint32(mValue.s_f.f);
        case TYPE_INT8:
            return quint32(mValue.s_i8.int8);
        case TYPE_UINT8:
            return quint32(mValue.s_ui8.uint8);
        case TYPE_INT16:
            return quint32(mValue.s_i16.int16);
        case TYPE_UINT16:
            return quint32(mValue.s_ui16.uint16);
        case TYPE_INT32:
            return quint32(mValue.s_i32.int32);
        case TYPE_UINT32:
            return quint32(mValue.s_ui32.uint32);
        default:
            return 0;
        }
    }

    u_data      mValue;
    ValueType   mType;
};

inline QDebug operator<<(QDebug debug, const DataValue v) {
    debug.nospace() << v.toString();
    return debug;
}

#endif // DATAVALUE_H

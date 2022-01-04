#include "curve_patron.h"

CurvePatron::CurvePatron(QObject *parent) : QObject(parent),
    mPenStyle(Qt::SolidLine),
    mColor(QColor(220,230,240)),
    mPenWidth(0)
{
    mPen.setColor(mColor);
    mPen.setWidth(0);
    mPen.setStyle(mPenStyle);
}

Qt::PenStyle CurvePatron::penStyle() const
{
    return mPenStyle;
}

void CurvePatron::setPenStyle(const Qt::PenStyle &penStyle)
{
    mPenStyle = penStyle;
    mPen.setStyle(mPenStyle);
}

QColor CurvePatron::color() const
{
    auto color = mColor;
    return mColor;
}

void CurvePatron::setColor(const QColor &color)
{
    mColor = color;
    mPen.setColor(mColor);
}

void CurvePatron::save(QSettings *settings)
{
    settings->beginGroup("Patron");
    settings->setValue("Name", mName);
    auto color = mColor;
    settings->setValue("Color", color.name(QColor::HexArgb));
    settings->setValue("PenStyle", int(mPenStyle));
    settings->setValue("PenWidth", int(mPenWidth));
    settings->beginWriteArray("Points");
    {
        for(int i=0; i< mXValues.count();i++) {
            settings->setArrayIndex(i);
            settings->setValue("X", mXValues.at(i));
            settings->setValue("Y", mYValues.at(i));
        }
    }
    settings->endArray();
    settings->endGroup();
}

void CurvePatron::load(QSettings *settings)
{

    settings->beginGroup("Patron");
    mName = settings->value("Name").toString();
    mColor = QColor(settings->value("Color").toString());
    mPenStyle = Qt::PenStyle(settings->value("PenStyle").toInt());
    mPenWidth = Qt::PenStyle(settings->value("PenWidth").toInt());
    mPen.setColor(mColor);
    mPen.setStyle(mPenStyle);
    mPen.setWidth(mPenWidth);
    clearPoints();
    int pointsCount = settings->beginReadArray("Points");
    {

        for(int i=0;i<pointsCount;i++) {
            settings->setArrayIndex(i);

            bool xOk, yOk;
            double x, y;
            x = settings->value("X").toDouble(&xOk);
            y = settings->value("Y").toDouble(&yOk);

            if(xOk && yOk)
                addPoint(x, y);
        }
    }
    settings->endArray();
    settings->endGroup();
}

int CurvePatron::penWidth() const
{
    return mPenWidth;
}

void CurvePatron::setPenWidth(int penWidth)
{
    mPenWidth = penWidth;
    mPen.setWidth(mPenWidth);
}

QString CurvePatron::name() const
{
    return mName;
}

void CurvePatron::setName(const QString &name)
{
    mName = name;
}

QPen CurvePatron::pen() const
{
    return mPen;
}

QList<double> CurvePatron::xValues() const
{
    return mXValues;
}

QList<double> CurvePatron::yValues() const
{
    return mYValues;
}

void CurvePatron::clearPoints()
{
    mXValues.clear();
    mYValues.clear();
}

void CurvePatron::addPoint(double x, double y)
{
    mXValues.append(x);
    mYValues.append(y);
}

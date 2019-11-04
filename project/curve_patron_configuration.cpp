#include "curve_patron_configuration.h"

QTBCurvePatronConfiguration::QTBCurvePatronConfiguration(QObject *parent) : QObject(parent),
    mPenStyle(Qt::SolidLine),
    mColor(QColor(220,230,240))
{
    mPen.setColor(mColor);
    mPen.setWidth(1);
    mPen.setStyle(mPenStyle);
}

Qt::PenStyle QTBCurvePatronConfiguration::penStyle() const
{
    return mPenStyle;
}

void QTBCurvePatronConfiguration::setPenStyle(const Qt::PenStyle &penStyle)
{
    mPenStyle = penStyle;
    mPen.setStyle(mPenStyle);
}

QColor QTBCurvePatronConfiguration::color() const
{
    return mColor;
}

void QTBCurvePatronConfiguration::setColor(const QColor &color)
{
    mColor = color;
    mPen.setColor(mColor);
}

void QTBCurvePatronConfiguration::save(QSettings *settings)
{

    settings->beginGroup("Patron");
    settings->setValue("Name", mName);
    settings->setValue("Color", mColor.name());
    settings->setValue("PenStyle", int(mPenStyle));
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

void QTBCurvePatronConfiguration::load(QSettings *settings)
{

    settings->beginGroup("Patron");
    mName = settings->value("Name").toString();
    mColor = QColor(settings->value("Color").toString());
    mPenStyle = Qt::PenStyle(settings->value("PenStyle").toInt());
    mPen.setColor(mColor);
    mPen.setStyle(mPenStyle);
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

QString QTBCurvePatronConfiguration::name() const
{
    return mName;
}

void QTBCurvePatronConfiguration::setName(const QString &name)
{
    mName = name;
}

QPen QTBCurvePatronConfiguration::pen() const
{
    return mPen;
}

QList<double> QTBCurvePatronConfiguration::xValues() const
{
    return mXValues;
}

QList<double> QTBCurvePatronConfiguration::yValues() const
{
    return mYValues;
}

void QTBCurvePatronConfiguration::clearPoints()
{
    mXValues.clear();
    mYValues.clear();
}

void QTBCurvePatronConfiguration::addPoint(double x, double y)
{
    mXValues.append(x);
    mYValues.append(y);
}

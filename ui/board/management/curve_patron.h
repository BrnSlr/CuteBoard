#ifndef CURVE_PATRON_CONFIGURATION_H
#define CURVE_PATRON_CONFIGURATION_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QPen>
#include "ui/ui_global.h"
#include "log4qt/logger.h"

class BOARD_UI_EXPORT CurvePatron : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CurvePatron(QObject *parent = nullptr);

    Qt::PenStyle penStyle() const;
    QColor color() const;
    QPen pen() const;
    QList<double> xValues() const;
    QList<double> yValues() const;
    QString name() const;
    int penWidth() const;

    void clearPoints();
    void addPoint(double x, double y);
    void setPenStyle(const Qt::PenStyle &penStyle);
    void setColor(const QColor &color);
    void setName(const QString &name);
    void setPenWidth(int penWidth);

    void save(QSettings *settings);
    void load(QSettings *settings);


signals:

public slots:

protected:
    QString mName;
    QList<double> mXValues;
    QList<double> mYValues;
    Qt::PenStyle mPenStyle;
    QColor mColor;
    QPen mPen;
    int mPenWidth;
};

#endif // CURVE_PATRON_CONFIGURATION_H

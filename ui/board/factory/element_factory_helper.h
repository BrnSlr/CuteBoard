#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include <iostream>
#include <memory>
#include <functional>
#include <map>
#include <QDebug>
#include "ui/board/board_element.h"
#include "element_interface.h"
#include "3rdparty/qpluginfactory.h"
#include "ui/ui_global.h"

using namespace std;

class BOARD_UI_EXPORT ElementFactory
{
public:
    static ElementFactory* Factory() {
        static ElementFactory factory;
        return &factory;
    }

    BoardElement* createElement(const QString &name);
    QString description(const QString &name);
    QString tooltip(const QString &name);
    QString category(const QString &name);
    QPixmap iconPixmap(const QString &name);
    QString iconPath(const QString &name);
    BoardElement::ElementType elementType(const QString &name);
    BoardElement::TimeSeriesType timeseriesType(const QString &name);
    BoardElement::TimeSeriesSize timeseriesSize(const QString &name);
    bool valid(const QString &name);
    QStringList pluginslist() const;

private:
};


#endif // ELEMENTFACTORY_H

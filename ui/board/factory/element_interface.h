#ifndef ELEMENT_INTERFACE_H
#define ELEMENT_INTERFACE_H

#include <QObject>
#include <QPixmap>
#include "ui/board/board_element.h"

class ElementInterface : public QObject
{
public:
    virtual inline ~ElementInterface() = default;

    virtual QPixmap iconPixmap(QString path) {return QPixmap(path);}
    virtual BoardElement * createElement() = 0;
};

#define ElementInterfaceIid "board.elementPlugin"

Q_DECLARE_INTERFACE(ElementInterface, ElementInterfaceIid)

#endif // ELEMENT_INTERFACE_H

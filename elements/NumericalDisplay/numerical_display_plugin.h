#ifndef NUMERICAL_DISPLAY_PLUGIN_H
#define NUMERICAL_DISPLAY_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "numerical_display.h"

class NumericalDisplayPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "numerical_display.json")
public:
    NumericalDisplayPlugin();
    virtual BoardElement * createElement() override;
};

#endif // NUMERICAL_DISPLAY_PLUGIN_H

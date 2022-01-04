#ifndef STATE_DISPLAY_PLUGIN_H
#define STATE_DISPLAY_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "gauge_vertical.h"

class GaugeVerticalPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "gauge_vertical.json")
public:
    GaugeVerticalPlugin();
    virtual BoardElement * createElement() override;
};

#endif // STATE_DISPLAY_PLUGIN_H

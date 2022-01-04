#ifndef GAUGE_RADIAL_PLUGIN_H
#define GAUGE_RADIAL_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "gauge_radial.h"

class GaugeRadialPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "gauge_radial.json")
public:
    GaugeRadialPlugin();
    virtual BoardElement * createElement() override;
};

#endif // GAUGE_RADIAL_PLUGIN_H

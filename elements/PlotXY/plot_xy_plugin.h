#ifndef PLOT_XY_PLUGIN_H
#define PLOT_XY_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "plot_xy.h"

class PlotXYPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "plot_xy.json")
public:
    PlotXYPlugin();
    virtual BoardElement * createElement() override;
};

#endif // PLOT_XY_PLUGIN_H

#ifndef PLOT_TIME_PLUGIN_H
#define PLOT_TIME_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "plot_time.h"

class PlotTimePlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "plot_time.json")
public:
    PlotTimePlugin();
    virtual BoardElement * createElement() override;
};

#endif // PLOT_TIME_PLUGIN_H

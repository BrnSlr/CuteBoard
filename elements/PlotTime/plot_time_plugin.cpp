#include "plot_time_plugin.h"

PlotTimePlugin::PlotTimePlugin()
{
    Q_INIT_RESOURCE(iconPlotTime);
}

BoardElement *PlotTimePlugin::createElement()
{
    return new PlotTime();
}

#include "plot_xy_plugin.h"

PlotXYPlugin::PlotXYPlugin()
{
    Q_INIT_RESOURCE(iconPlotXY);
}

BoardElement *PlotXYPlugin::createElement()
{
    return new PlotXY();
}

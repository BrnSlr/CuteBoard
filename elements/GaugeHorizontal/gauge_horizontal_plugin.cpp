#include "gauge_horizontal_plugin.h"

GaugeHorizontalPlugin::GaugeHorizontalPlugin()
{
    Q_INIT_RESOURCE(iconGaugeHorizontal);
}

BoardElement *GaugeHorizontalPlugin::createElement()
{
    return new GaugeHorizontal();
}

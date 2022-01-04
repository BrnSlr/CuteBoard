#include "gauge_vertical_plugin.h"

GaugeVerticalPlugin::GaugeVerticalPlugin()
{
    Q_INIT_RESOURCE(iconGaugeVertical);
}

BoardElement *GaugeVerticalPlugin::createElement()
{
    return new GaugeVertical();
}

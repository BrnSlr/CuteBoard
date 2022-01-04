#include "gauge_radial_plugin.h"

GaugeRadialPlugin::GaugeRadialPlugin()
{
    Q_INIT_RESOURCE(iconGaugeRadial);
}

BoardElement *GaugeRadialPlugin::createElement()
{
    return new GaugeRadial();
}

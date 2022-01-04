#include "alarm_panel_plugin.h"

AlarmPanelPlugin::AlarmPanelPlugin()
{
    Q_INIT_RESOURCE(iconAlarmPanel);
}

BoardElement *AlarmPanelPlugin::createElement()
{
    return new AlarmPanel();
}

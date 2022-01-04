#include "state_display_plugin.h"

StateDisplayPlugin::StateDisplayPlugin()
{
    Q_INIT_RESOURCE(iconStateDisplay);
}

BoardElement *StateDisplayPlugin::createElement()
{
    return new StateDisplay();
}

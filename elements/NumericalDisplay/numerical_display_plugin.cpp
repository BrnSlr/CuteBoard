#include "numerical_display_plugin.h"

NumericalDisplayPlugin::NumericalDisplayPlugin()
{
    Q_INIT_RESOURCE(iconNumericalDisplay);
}

BoardElement *NumericalDisplayPlugin::createElement()
{
    return new NumericalDisplay();
}

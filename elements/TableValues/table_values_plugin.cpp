#include "table_values_plugin.h"

TableValuesPlugin::TableValuesPlugin()
{
    Q_INIT_RESOURCE(iconTableValues);
}

BoardElement *TableValuesPlugin::createElement()
{
    return new TableValues();
}

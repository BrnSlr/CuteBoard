#ifndef TABLE_VALUES_PLUGIN_H
#define TABLE_VALUES_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "table_values.h"

class TableValuesPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "table_values.json")
public:
    TableValuesPlugin();
    virtual BoardElement * createElement() override;
};

#endif // TABLE_VALUES_PLUGIN_H

#ifndef ALARM_PANEL_PLUGIN_H
#define ALARM_PANEL_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "alarm_panel.h"

class AlarmPanelPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "alarm_panel.json")
public:
    AlarmPanelPlugin();
    virtual BoardElement * createElement() override;
};

#endif // ALARM_PANEL_PLUGIN_H

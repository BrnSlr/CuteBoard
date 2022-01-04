#ifndef TEXT_LABEL_PLUGIN_H
#define TEXT_LABEL_PLUGIN_H

#include "ui/board/factory/element_interface.h"
#include "text_label.h"

class TextLabelPlugin : public ElementInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_INTERFACES(ElementInterface)
    Q_PLUGIN_METADATA(IID ElementInterfaceIid FILE "text_label.json")
public:
    TextLabelPlugin();
    virtual BoardElement * createElement() override;
};

#endif // TEXT_LABEL_PLUGIN_H

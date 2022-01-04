#include "text_label_plugin.h"

TextLabelPlugin::TextLabelPlugin()
{
    Q_INIT_RESOURCE(iconTextLabel);
}

BoardElement *TextLabelPlugin::createElement()
{
    return new TextLabel();
}

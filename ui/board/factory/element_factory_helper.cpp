#include "element_factory_helper.h"

Q_GLOBAL_PLUGIN_FACTORY(ElementInterface, "element", _loader)

BoardElement *ElementFactory::createElement(const QString &name)
{
    BoardElement* instance = nullptr;

    if(valid(name)) {
        auto plugin = _loader->plugin(name);
        if(plugin) {
            instance = plugin->createElement();
            instance->setName(name);
            instance->setType(elementType(name));
            instance->setTimeSeriesSize(timeseriesSize(name));
            instance->setTimeSeriesType(timeseriesType(name));
        }
    }

    return instance;
}

QString ElementFactory::description(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto descr = meta[QStringLiteral("Description")].toString();
    return descr;
}

QString ElementFactory::tooltip(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto tooltip = meta[QStringLiteral("ToolTip")].toString();
    return tooltip;
}

QString ElementFactory::category(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto tooltip = meta[QStringLiteral("Category")].toString();
    return tooltip;
}

QPixmap ElementFactory::iconPixmap(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto iconPath = meta[QStringLiteral("IconPath")].toString();

    QPixmap pix;
    auto plugin = _loader->plugin(name);
    if(plugin)
        pix = plugin->iconPixmap(iconPath).scaled(25, 25, Qt::KeepAspectRatio);
    return pix;
}

QString ElementFactory::iconPath(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto iconPath = meta[QStringLiteral("IconPath")].toString();

    return iconPath;
}

BoardElement::ElementType ElementFactory::elementType(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto typeInt = meta[QStringLiteral("Type")].toInt();
    return BoardElement::ElementType(typeInt);
}

BoardElement::TimeSeriesType ElementFactory::timeseriesType(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto typeInt = meta[QStringLiteral("TimeSeriesType")].toInt();
    return BoardElement::TimeSeriesType(typeInt);
}

BoardElement::TimeSeriesSize ElementFactory::timeseriesSize(const QString &name)
{
    auto meta = _loader->metaData(name);
    auto typeInt = meta[QStringLiteral("TimeSeriesSize")].toInt();
    return BoardElement::TimeSeriesSize(typeInt);
}

bool ElementFactory::valid(const QString &name)
{
    auto meta = _loader->metaData(name);
    if(meta.contains(QStringLiteral("Validity")))
        return meta[QStringLiteral("Validity")].toBool();
    return true;
}

QStringList ElementFactory::pluginslist() const
{
    return _loader->allKeys();
}

#include "qpluginfactory.h"

#include <QFileInfo>
#include <QDebug>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>
#include <QLibraryInfo>

namespace {

class StaticPluginInfo : public QPluginFactoryBase::PluginInfo
{
public:
    StaticPluginInfo(QStaticPlugin plugin);

    QJsonObject metaData() const override;
    QObject *instance() override;
    bool isLoaded() const override;
    void unload(const QString &key) override;

private:
    QStaticPlugin _plugin;
};

#if QT_CONFIG(library)

class DynamicPluginInfo : public QPluginFactoryBase::PluginInfo
{
public:
    DynamicPluginInfo(QScopedPointer<QPluginLoader, QScopedPointerDeleteLater> &loader);

    QJsonObject metaData() const override;
    QObject *instance() override;
    bool isLoaded() const override;
    void unload(const QString &key) override;

private:
    QScopedPointer<QPluginLoader, QScopedPointerDeleteLater> _loader;
};

#endif

}



QPluginFactoryBase::QPluginFactoryBase(QString pluginType, QObject *parent, bool isDebugBuild) :
    QPluginFactoryBase(std::move(pluginType), QByteArray(), parent, isDebugBuild)
{}

QPluginFactoryBase::QPluginFactoryBase(QString pluginType, QByteArray pluginIid, QObject *parent, bool isDebugBuild) :
    QObject{parent},
    _isDebugBuild{isDebugBuild},
    _pluginType{std::move(pluginType)},
    _pluginIid{std::move(pluginIid)}
{
    //setup dynamic plugins
    reloadPlugins();
}

bool QPluginFactoryBase::addSearchDir(const QDir &dir, bool isTopLevel)
{
    if(_extraDirs.contains(dir))
        return false;

    if(isTopLevel) {
        auto mDir = dir;
        if(mDir.cd(_pluginType))
            _extraDirs.append(mDir);
    }  else
        _extraDirs.append(dir);

    return true;
}

QStringList QPluginFactoryBase::allKeys() const
{
    QMutexLocker _{&_loaderMutex};
    return _plugins.keys();
}

QJsonObject QPluginFactoryBase::metaData(const QString &key) const
{
    QMutexLocker _{&_loaderMutex};
    auto info = _plugins.value(key);
    if(info)
        return info->metaData()[QStringLiteral("MetaData")].toObject();
    else
        return {};
}

QObject *QPluginFactoryBase::plugin(const QString &key) const
{
    QMutexLocker _{&_loaderMutex};
    auto info = _plugins.value(key);
    if(info)
        return info->instance();
    else
        return nullptr;
}

QString QPluginFactoryBase::pluginType() const
{
    return _pluginType;
}

QByteArray QPluginFactoryBase::pluginIid() const
{
    return _pluginIid;
}

void QPluginFactoryBase::setPluginIid(const QByteArray &pluginIid)
{
    _pluginIid = pluginIid;
    reloadPlugins();
}

void QPluginFactoryBase::reloadPlugins()
{
    QMutexLocker _{&_loaderMutex};

    //find the plugin dir
    auto oldKeys = _plugins.keys();

#if QT_CONFIG(library)

    QList<QDir> allDirs;
    //first: dirs in path
    //MAJOR remove
    auto envVar = QStringLiteral("PLUGIN_%1_PATH").arg(_pluginType.toUpper()).toUtf8();
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    auto path = qEnvironmentVariable(envVar.constData());
#else
    auto path = QString::fromUtf8(qgetenv(envVar.constData()));
#endif
    for(const auto &p : path.split(QDir::listSeparator(), QString::SkipEmptyParts)) {
        QDir plgDir{p};
        if(plgDir.exists())
            allDirs.append(plgDir);
    }

    //second: extra dirs
    allDirs.append(_extraDirs);

    //third: original plugin dirs
    for(const auto &plgPath : QCoreApplication::libraryPaths()) {
        QDir plgDir{plgPath};
        if(plgDir.cd(_pluginType))
            allDirs.append(plgDir);
    }

    const auto appPath = QCoreApplication::applicationDirPath();
    QDir appDir{appPath};
    if(appDir.exists())
        allDirs.append(appDir);

    //third: original plugin dirs
    const auto libPath = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
    QDir libDir{libPath};
    if(libDir.exists())
        allDirs.append(libDir);

    //setup dynamic plugins
    for(auto &pluginDir : allDirs) {
#ifdef Q_OS_WIN
        for(const auto &info : pluginDir.entryInfoList({QStringLiteral("*.dll")}, QDir::Files | QDir::Readable)) {
#else
        for(const auto &info : pluginDir.entryInfoList(QDir::Files | QDir::Readable)) {
#endif
            QScopedPointer<QPluginLoader, QScopedPointerDeleteLater> loader{new QPluginLoader{info.absoluteFilePath()}};
            auto metaData = loader->metaData();
            auto keys = checkMeta(metaData, loader->fileName());
            if(keys.isEmpty()) {
                continue;
            }

            auto dynInfo = QSharedPointer<DynamicPluginInfo>::create(loader);
            for(const auto key : keys) {
                auto k = key.toString();
                if(!_plugins.contains(k))
                    _plugins.insert(k, dynInfo);
                oldKeys.removeOne(k);
            }
        }
    }

#endif

    //setup static plugins
    for(const auto &info : QPluginLoader::staticPlugins()) {
        auto keys = checkMeta(info.metaData(), QString());
        for(const auto key : keys) {
            auto k = key.toString();
            if(!_plugins.contains(k))
                _plugins.insert(k, QSharedPointer<StaticPluginInfo>::create(info));
            oldKeys.removeOne(k);
        }
    }

    //remove old, now unused plugins
    for(const auto &key : oldKeys)
        _plugins.remove(key);
}

bool QPluginFactoryBase::isLoaded(const QString &key) const
{
    QMutexLocker _{&_loaderMutex};
    auto info = _plugins.value(key);
    if(info)
        return info->isLoaded();
    else
        return false;
}

void QPluginFactoryBase::unload(const QString &key)
{
    QMutexLocker _{&_loaderMutex};
    auto info = _plugins.value(key);
    if(info)
        info->unload(key);
}

QJsonArray QPluginFactoryBase::checkMeta(const QJsonObject &metaData, const QString &filename) const
{
    if(metaData[QStringLiteral("debug")].toBool() != _isDebugBuild)
        return {};

    auto iid = metaData.value(QStringLiteral("IID")).toString().toUtf8();
    if(!_pluginIid.isNull() && iid != _pluginIid) {
        //        qWarning().noquote() << "File" << filename << "is no plugin of type" << _pluginIid;
        return {};
    }

    auto data = metaData[QStringLiteral("MetaData")].toObject();
    auto keys = data[QStringLiteral("Keys")].toArray();
    if(keys.isEmpty()) {
        logger()->warn() << "Plugin" << filename << "does not provide any Keys";
        return {};
    }

//    logger()->info() << "Plugin " << _pluginIid << " loaded : " << filename;

    return keys;
}

StaticPluginInfo::StaticPluginInfo(QStaticPlugin plugin) :
    _plugin{plugin}
{}

QJsonObject StaticPluginInfo::metaData() const
{
    return _plugin.metaData();
}

QObject *StaticPluginInfo::instance()
{
    return _plugin.instance();
}

bool StaticPluginInfo::isLoaded() const
{
    return true;
}

void StaticPluginInfo::unload(const QString &key)
{
    Q_UNUSED(key)
}



#if QT_CONFIG(library)

DynamicPluginInfo::DynamicPluginInfo(QScopedPointer<QPluginLoader, QScopedPointerDeleteLater> &loader)
{
    _loader.swap(loader);
}

QJsonObject DynamicPluginInfo::metaData() const
{
    return _loader->metaData();
}

QObject *DynamicPluginInfo::instance()
{
    if(!_loader->isLoaded() && !_loader->load())
        qWarning().noquote() << "Failed to load plugin"
                             << _loader->fileName()
                             << " with error:"
                             << _loader->errorString();
    return _loader->instance();
}

bool DynamicPluginInfo::isLoaded() const
{
    return _loader->isLoaded();
}

void DynamicPluginInfo::unload(const QString &key)
{
    if(_loader->isLoaded()){
        if(!_loader->unload())
            qWarning().noquote() << "Failed to unload plugin for key" << key << "with error:" << _loader->errorString();
    }
}

#endif

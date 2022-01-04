#include "ui/board/management/project.h"

Project::Project(QObject *parent) : QObject (parent),
    mPagesDirWrittable(true)
{
    //    mAlarmsWatcher = new QFileSystemWatcher(this);
    //    connect(mAlarmsWatcher, &QFileSystemWatcher::fileChanged, this, &Project::alarmFilesChanged);
    //    connect(mAlarmsWatcher, &QFileSystemWatcher::directoryChanged, this, &Project::alarmFilesChanged);
}

Project::~Project()
{
    clear();

    //    delete mAlarmsWatcher;
}

bool Project::generate(const QString& workingDirectory, const QString& projectName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    bool status = false;
    clear();

    mProjectPath = QDir::toNativeSeparators(workingDirectory + QDir::separator() + projectName);

    QDir dirWorking(workingDirectory);

    if(dirWorking.mkdir(projectName) ) {
        QFileInfo dirInfo(mProjectPath);
        if(dirInfo.isWritable()) {
            mSettingsPath = mProjectPath + QDir::separator() + projectName + QString(".project");

            QSettings settings(mSettingsPath, QSettings::IniFormat);
            settings.sync();

            setCreationDate(QDateTime::currentDateTime());
            setModificationDate(QDateTime::currentDateTime());
            setName(projectName);

            mPagesPath = mProjectPath + QDir::separator() + QString("Pages");
            mPagesDirWrittable = true;

            QDir dirPages(mPagesPath);
            if (!dirPages.exists())
                dirPages.mkpath(".");

            mParamSettingsPath = mProjectPath + QDir::separator() + QString("Parameters");
            mParamSettingsDirWrittable = true;

            QDir dirParamSettings(mParamSettingsPath);
            if (!dirParamSettings.exists())
                dirParamSettings.mkpath(".");


            mAlarmsConfigPath = mProjectPath + QDir::separator() + QString("Alarms");

            QDir dirAlarmConfigs(mAlarmsConfigPath);
            if (!dirAlarmConfigs.exists())
                dirAlarmConfigs.mkpath(".");

            mResourcesPath = mProjectPath + QDir::separator() + QString("Resources") ;

            QDir dirResources(mResourcesPath);
            if (!dirResources.exists())
                dirResources.mkpath(".");

            mResourcesImagesPath = mResourcesPath + QDir::separator() + QString("Images") ;

            QDir dirResourcesImages(mResourcesImagesPath);
            if (!dirResourcesImages.exists())
                dirResourcesImages.mkpath(".");

            status = true;
        }
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return status;
}

bool Project::load(const QString& workingDirectory, const QString& projectName, bool fullLoad)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    bool status= false;
    clear();

    mProjectPath = QDir::toNativeSeparators(workingDirectory + QDir::separator() + projectName);
    mSettingsPath = mProjectPath + QDir::separator() + projectName + QString(".project");

    QFileInfo fileInfo(mSettingsPath);
    if(fileInfo.exists()) {

        QSettings settings(mSettingsPath, QSettings::IniFormat);

        if(settings.contains(PRO_SETTINGS_KEY_CREATIONDATE))
            mCreationDate = settings.value(PRO_SETTINGS_KEY_CREATIONDATE).toDateTime();

        if(settings.contains(PRO_SETTINGS_KEY_MODIFICATIONDATE))
            mModificationDate = settings.value(PRO_SETTINGS_KEY_MODIFICATIONDATE).toDateTime();

        if(settings.contains(PRO_SETTINGS_KEY_NAME))
            mName = settings.value(PRO_SETTINGS_KEY_NAME).toString();

        mPagesPath = mProjectPath + QDir::separator() + QString("Pages");

        QDir dirPages(mPagesPath);
        if (!dirPages.exists()) {
            if(dirPages.mkpath("."))
                mPagesDirWrittable = true;
            else
                mPagesDirWrittable = false;
        } else {
            QFileInfo dirInfo(mPagesPath);
            if(dirInfo.isWritable())
                mPagesDirWrittable = true;
            else
                mPagesDirWrittable = false;
        }


        mParamSettingsPath = mProjectPath + QDir::separator() + QString("Parameters");

        QDir dirVisuParam(mParamSettingsPath);
        if (!dirVisuParam.exists()) {
            if(dirVisuParam.mkpath("."))
                mParamSettingsDirWrittable = true;
            else
                mParamSettingsDirWrittable = false;
        } else {
            QFileInfo dirInfo(mParamSettingsPath);
            if(dirInfo.isWritable())
                mParamSettingsDirWrittable = true;
            else
                mParamSettingsDirWrittable = false;
        }


        mAlarmsConfigPath = mProjectPath + QDir::separator() + QString("Alarms");

        QDir dirAlarmConfigs(mAlarmsConfigPath);
        if (!dirAlarmConfigs.exists())
            dirAlarmConfigs.mkpath(".");

        //        auto alarmDirectories = mAlarmsWatcher->directories();
        //        if(alarmDirectories.count() > 0)
        //            mAlarmsWatcher->removePaths(alarmDirectories);
        //        mAlarmsWatcher->addPath(mAlarmsConfigPath);

        if(fullLoad) {
            loadPages();
            loadParametersSettings();
            loadAlarmsConfigurations();

            mName = projectName;

            emit loaded();
        }

        mResourcesPath = mProjectPath + QDir::separator() + QString("Resources") ;

        QDir dirResources(mResourcesPath);
        if (!dirResources.exists())
            dirResources.mkpath(".");

        mResourcesImagesPath = mResourcesPath + QDir::separator() + QString("Images") ;

        QDir dirResourcesImages(mResourcesImagesPath);
        if (!dirResourcesImages.exists())
            dirResourcesImages.mkpath(".");

        status = true;
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return status;
}

void Project::clear()
{
    mName.clear();
    mPages.clear();
    mParametersSettings.clear();
    mAlarmsConfigurations.clear();
}

void Project::fullReload()
{
    loadPages();
    loadParametersSettings();
    loadAlarmsConfigurations();

    emit loaded();
}

void Project::loadPages()
{
    mPages.clear();

    QDir directory(mPagesPath);
    QStringList pages = directory.entryList(QStringList() << "*.page" ,QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    foreach(QString filename, pages) {

        QString pageSettingsPath = mPagesPath + QDir::separator() + filename ;
        QFileInfo fi(pageSettingsPath);
        QString base = fi.completeBaseName();

        auto page = QExplicitlySharedDataPointer<Page>(new Page(base, mPagesPath));
        page->loadPageInformation();
        mPages.insert(page->name(), page);
    }

    emit updated();
    logger()->info() << QString("%1 loaded pages").arg(mPages.count());
}

void Project::requestPage(const QString& pageName)
{
    if(mPages.contains(pageName))
    {
        mRequestedPageName = pageName;
        emit pageRequested();
    }
}

//void Project::alarmFilesChanged(QString path)
//{
//    loadAlarmsConfigurations();

//    emit pageRequested();
//}

QString Project::resourcesPath() const
{
    return mResourcesPath;
}

QString Project::resourcesImagesPath() const
{
    return mResourcesImagesPath;
}

QString Project::alarmsConfigPath() const
{
    return mAlarmsConfigPath;
}

QMap<QString, QExplicitlySharedDataPointer<Alarms> > Project::alarmsConfigurations() const
{
    return mAlarmsConfigurations;
}

QExplicitlySharedDataPointer<Alarms> Project::alarmConfiguration(const QString &name)
{
    if(mAlarmsConfigurations.contains(name)) {
        return mAlarmsConfigurations.value(name);
    }
    logger()->warn() << QString("Alarm configuration does not exist - %1").arg(name);
    return QExplicitlySharedDataPointer<Alarms>();
}

QString Project::requestedPageName() const
{
    return mRequestedPageName;
}

void Project::setRequestedPageName(const QString &requestedPageName)
{
    mRequestedPageName = requestedPageName;
}

QList<ElementDecl> Project::findParameter(QString parameterName)
{
    QList<ElementDecl> result;

    QStringList pagesNames = mPages.keys();

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(pagesNames.begin(),
              pagesNames.end(),
              collator);

    for(auto pageName : pagesNames) {
        auto path = mPagesPath + QDir::separator() + pageName + QString(".page");

        QSettings settings(path, QSettings::IniFormat);

        int elementsCount = settings.beginReadArray("Elements");

        for(int i=0;i<elementsCount;i++) {
            settings.setArrayIndex(i);
            QString elementName = settings.value("name").toString();

            settings.beginGroup("ElementGeometry");
            auto col = settings.value("column").toInt();
            auto row = settings.value("row").toInt();
            settings.endGroup();

            settings.beginGroup("ElementConfigurations");
            int paramCount = settings.beginReadArray("Parameters");
            for(int i= 0; i< paramCount;i++) {
                settings.setArrayIndex(i);

                QString paramlabel;
                auto shared = settings.value("Connected").toBool();
                if(shared) {
                    paramlabel = settings.value("ConnectedParamLabel").toString();
                } else {
                    paramlabel= settings.value("Label").toString();
                }

                if(paramlabel.contains(parameterName, Qt::CaseInsensitive)) {
                    ElementDecl el;
                    el.row = row;
                    el.column = col;
                    el.parameter = paramlabel;
                    el.page_name = pageName;
                    el.type_name = elementName;

                    result.append(el);
                }
            }
            settings.endArray();
            settings.endGroup();
        }
        settings.endArray();
    }

    return result;
}

void Project::setCurrentPageName(const QString &currentPageName)
{
    mCurrentPageName = currentPageName;
    emit pageLoaded();
}

QString Project::pagesPath() const
{
    return mPagesPath;
}

bool Project::addPage(QExplicitlySharedDataPointer<Page> page)
{    
    if(mPagesDirWrittable) {
        if(page) {
            mPages.insert(page->name(), page);
            setModificationDate(QDateTime::currentDateTime());
            emit pagesListUpdated();

            return true;
        }
    }
    return false;
}

QExplicitlySharedDataPointer<Page> Project::page(const QString& pageName)
{
    if(mPages.contains(pageName))
        return mPages.value(pageName);
    else {
        return QExplicitlySharedDataPointer<Page>();
    }
}

QExplicitlySharedDataPointer<Page> Project::currentPage()
{
    if(mPages.contains(mCurrentPageName))
        return mPages.value(mCurrentPageName);
    else {
        return QExplicitlySharedDataPointer<Page>();
    }
}

QExplicitlySharedDataPointer<Page> Project::requestedPage()
{
    if(mPages.contains(mRequestedPageName))
        return mPages.value(mRequestedPageName);
    else {
        return QExplicitlySharedDataPointer<Page>();
    }
}

void Project::loadParametersSettings()
{
    mParametersSettings.clear();

    QDir directory(mParamSettingsPath);
    QStringList files = directory.entryList(QStringList() << "*.param" ,QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    foreach(QString filename, files) {

        QString parameterSettingsPath = mParamSettingsPath + QDir::separator() + filename ;
        QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings = QExplicitlySharedDataPointer<ParameterConfiguration>(new ParameterConfiguration());

        parameterSettings->loadFromFile(parameterSettingsPath);

        if(!parameterSettings->label().isEmpty() && !parameterSettings->description().isEmpty()) {
            if(mParametersSettings.value(parameterSettings->label()).contains(parameterSettings->description())) {
                auto conf = mParametersSettings.value(parameterSettings->label()).value(parameterSettings->description());
                logger()->warn() << QString("Parameter configuration conflict  : File %1 - File : %2").arg(QFileInfo(conf->saveFilePath()).fileName()).arg(QFileInfo(parameterSettings->saveFilePath()).fileName());
            } else {
                mParametersSettings[parameterSettings->label()].insert(parameterSettings->description(), parameterSettings);
            }
        } else {
            logger()->warn() << QString("Parameter configuration is not valid (empty string) - Label  : %1 - Descr : %2 - File : %3").arg(parameterSettings->label()).arg(parameterSettings->description()).arg(filename);
        }
    }

    logger()->info() << QString("%1 loaded parameter configurations").arg(mParametersSettings.count());
}

void Project::addParameterSettings(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    mParametersSettings[parameterSettings->label()].insert(parameterSettings->description(), parameterSettings);

    setModificationDate(QDateTime::currentDateTime());

    emit updated();
}

QExplicitlySharedDataPointer<ParameterConfiguration> Project::parameterSettings(const QString& name, const QString& description)
{
    if(mParametersSettings.contains(name)) {
        if(mParametersSettings.value(name).contains(description)) {
            return mParametersSettings.value(name).value(description);
        }
    }
    logger()->warn() << QString("Parameter configuration does not exist - %1 - %2").arg(name).arg(description);
    return QExplicitlySharedDataPointer<ParameterConfiguration>();
}

QDateTime Project::creationDate() const
{
    return mCreationDate;
}

void Project::setCreationDate(const QDateTime &creationDate)
{
    mCreationDate = creationDate;
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.setValue(PRO_SETTINGS_KEY_CREATIONDATE, creationDate);
}

QDateTime Project::modificationDate() const
{
    return mModificationDate;
}

void Project::setModificationDate(const QDateTime &modificationDate)
{
    mModificationDate = modificationDate;
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.setValue(PRO_SETTINGS_KEY_MODIFICATIONDATE, modificationDate);
}

QString Project::name() const
{
    return mName;
}

void Project::setName(const QString &name)
{
    mName = name;
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.setValue(PRO_SETTINGS_KEY_NAME, name);
}

QString Project::paramSettingsPath() const
{
    return mParamSettingsPath;
}

QString Project::currentPageName() const
{
    return mCurrentPageName;
}

QMap<QString, QMap<QString, QExplicitlySharedDataPointer<ParameterConfiguration> > > Project::parametersSettings() const
{
    return mParametersSettings;
}

void Project::loadAlarmsConfigurations()
{
    //    auto alarms = mAlarmsWatcher->files();
    //    if(alarms.count() > 0)
    //        mAlarmsWatcher->removePaths(alarms);
    //    mAlarmsConfigurations.clear();

    QDir directory(mAlarmsConfigPath);
    QStringList files = directory.entryList(QStringList() << "*.alrm" ,QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    foreach(QString filename, files) {

        QString alarmConfigPath = mAlarmsConfigPath + QDir::separator() + filename ;
        //        mAlarmsWatcher->addPath(alarmConfigPath);
        QExplicitlySharedDataPointer<Alarms> alarmConfig = QExplicitlySharedDataPointer<Alarms>(new Alarms());

        alarmConfig->loadFromFile(alarmConfigPath);
        mAlarmsConfigurations.insert(alarmConfig->name(), alarmConfig);
    }

    logger()->info() << QString("%1 loaded alarm configurations").arg(mAlarmsConfigurations.count());
}

void Project::addAlarmConfiguration(QExplicitlySharedDataPointer<Alarms> alarmConfig)
{
    mAlarmsConfigurations.insert(alarmConfig->name(), alarmConfig);
    setModificationDate(QDateTime::currentDateTime());

    emit updated();
}

QMap<QString, QExplicitlySharedDataPointer<Page> > Project::pages() const
{
    return mPages;
}

bool Project::exportToCsv(QString filePath)
{
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly) && !filePath.isNull()) {

        QTextStream stream(&file);
        stream << "Page;Parameter;Element;\n";
        QStringList pagesNames = mPages.keys();

        QCollator collator;
        collator.setNumericMode(true);

        std::sort(pagesNames.begin(),
                  pagesNames.end(),
                  collator);

        for(auto pageName : pagesNames) {
            auto path = mPagesPath + QDir::separator() + pageName + QString(".page");

            QSettings settings(path, QSettings::IniFormat);

            int elementsCount = settings.beginReadArray("Elements");

            for(int i=0;i<elementsCount;i++) {
                settings.setArrayIndex(i);
                QString elementName = settings.value("name").toString();

                settings.beginGroup("ElementConfigurations");
                int paramCount = settings.beginReadArray("Parameters");
                for(int i= 0; i< paramCount;i++) {
                    settings.setArrayIndex(i);

                    QString paramlabel;
                    auto shared = settings.value("Connected").toBool();
                    if(shared) {
                        paramlabel = settings.value("ConnectedParamLabel").toString();
                    } else {
                        paramlabel= settings.value("Label").toString();
                    }

                    stream << QString("%1;%2;%3;\n").arg(pageName).arg(paramlabel).arg(elementName);

                }
                settings.endArray();
                settings.endGroup();
            }
            settings.endArray();
        }

        file.close();
        return true;
    }
    return false;
}


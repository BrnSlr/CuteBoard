#include "project.h"

QTBProject::QTBProject(QObject *parent) : QObject (parent),
    mPagesDirWrittable(true)
{
}

QTBProject::~QTBProject()
{
    clear();
}

bool QTBProject::generate(const QString& workingDirectory, const QString& projectName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    bool status = false;
    clear();

    mProjectPath = QDir::toNativeSeparators(workingDirectory + QDir::separator() + projectName);

    QDir dirWorking(workingDirectory);

    if(dirWorking.mkdir(projectName)) {
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

            status = true;
        }
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return status;
}

bool QTBProject::load(const QString& workingDirectory, const QString& projectName, bool fullLoad)
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

        if(fullLoad) {
            loadPages();
            loadParametersSettings();
            loadAlarmsConfigurations();

            emit loaded();
        }

        status = true;
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return status;
}

void QTBProject::clear()
{
    mName.clear();
    qDeleteAll(mPages);
    mPages.clear();

    mParametersSettings.clear();
}

void QTBProject::loadPages()
{
    qDeleteAll(mPages);
    mPages.clear();

    QDir directory(mPagesPath);
    QStringList pages = directory.entryList(QStringList() << "*.page" ,QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    foreach(QString filename, pages) {

        QString pageSettingsPath = mPagesPath + QDir::separator() + filename ;
        QFileInfo fi(pageSettingsPath);
        QString base = fi.completeBaseName();

        QTBPage *page = new QTBPage(base, mPagesPath);
        page->loadPageInformation();
        mPages.insert(page->name(), page);
    }
}

void QTBProject::requestPage(const QString& pageName)
{
    if(mPages.contains(pageName))
    {
        mRequestedPageName = pageName;
        emit pageRequested();
    }
}

QString QTBProject::alarmsConfigPath() const
{
    return mAlarmsConfigPath;
}

QMap<QString, QExplicitlySharedDataPointer<QTBAlarmConfiguration> > QTBProject::alarmsConfigurations() const
{
    return mAlarmsConfigurations;
}

QExplicitlySharedDataPointer<QTBAlarmConfiguration> QTBProject::alarmConfiguration(const QString &name)
{
    if(mAlarmsConfigurations.contains(name)) {
        return mAlarmsConfigurations.value(name);
    }
    return QExplicitlySharedDataPointer<QTBAlarmConfiguration>();
}

QString QTBProject::requestedPageName() const
{
    return mRequestedPageName;
}

void QTBProject::setRequestedPageName(const QString &requestedPageName)
{
    mRequestedPageName = requestedPageName;
}

void QTBProject::setCurrentPageName(const QString &currentPageName)
{
    mCurrentPageName = currentPageName;
    emit pageLoaded();
}

QString QTBProject::pagesPath() const
{
    return mPagesPath;
}

bool QTBProject::addPage(QTBPage *page)
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

QTBPage *QTBProject::page(const QString& pageName)
{
    if(mPages.contains(pageName))
        return mPages.value(pageName);
    else {
        return nullptr;
    }
}

QTBPage *QTBProject::currentPage()
{
    if(mPages.contains(mCurrentPageName))
        return mPages.value(mCurrentPageName);
    else {
        return nullptr;
    }
}

QTBPage *QTBProject::requestedPage()
{
    if(mPages.contains(mRequestedPageName))
        return mPages.value(mRequestedPageName);
    else {
        return nullptr;
    }
}

void QTBProject::loadParametersSettings()
{
    mParametersSettings.clear();

    QDir directory(mParamSettingsPath);
    QStringList files = directory.entryList(QStringList() << "*.param" ,QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    foreach(QString filename, files) {

        QString parameterSettingsPath = mParamSettingsPath + QDir::separator() + filename ;
        QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings = QExplicitlySharedDataPointer<QTBParameterConfiguration>(new QTBParameterConfiguration());

        parameterSettings->loadFromFile(parameterSettingsPath);
        mParametersSettings[parameterSettings->label()].insert(parameterSettings->description(), parameterSettings);
    }
}

void QTBProject::addParameterSettings(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    mParametersSettings[parameterSettings->label()].insert(parameterSettings->description(), parameterSettings);

    setModificationDate(QDateTime::currentDateTime());
}

QExplicitlySharedDataPointer<QTBParameterConfiguration> QTBProject::parameterSettings(const QString& name, const QString& description)
{
    if(mParametersSettings.contains(name)) {
        if(mParametersSettings.value(name).contains(description)) {
            return mParametersSettings.value(name).value(description);
        }
    }
    return QExplicitlySharedDataPointer<QTBParameterConfiguration>();
}

QDateTime QTBProject::creationDate() const
{
    return mCreationDate;
}

void QTBProject::setCreationDate(const QDateTime &creationDate)
{
    mCreationDate = creationDate;
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.setValue(PRO_SETTINGS_KEY_CREATIONDATE, creationDate);
}

QDateTime QTBProject::modificationDate() const
{
    return mModificationDate;
}

void QTBProject::setModificationDate(const QDateTime &modificationDate)
{
    mModificationDate = modificationDate;
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.setValue(PRO_SETTINGS_KEY_MODIFICATIONDATE, modificationDate);
}

QString QTBProject::name() const
{
    return mName;
}

void QTBProject::setName(const QString &name)
{
    mName = name;
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.setValue(PRO_SETTINGS_KEY_NAME, name);
}

QString QTBProject::paramSettingsPath() const
{
    return mParamSettingsPath;
}

QString QTBProject::currentPageName() const
{
    return mCurrentPageName;
}

QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration> > > QTBProject::parametersSettings() const
{
    return mParametersSettings;
}

void QTBProject::loadAlarmsConfigurations()
{
    mAlarmsConfigurations.clear();

    QDir directory(mAlarmsConfigPath);
    QStringList files = directory.entryList(QStringList() << "*.alrm" ,QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    foreach(QString filename, files) {

        QString alarmConfigPath = mAlarmsConfigPath + QDir::separator() + filename ;
        QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig = QExplicitlySharedDataPointer<QTBAlarmConfiguration>(new QTBAlarmConfiguration());

        alarmConfig->loadFromFile(alarmConfigPath);
        mAlarmsConfigurations.insert(alarmConfig->name(), alarmConfig);
    }
}

void QTBProject::addAlarmConfiguration(QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig)
{
    mAlarmsConfigurations.insert(alarmConfig->name(), alarmConfig);
    setModificationDate(QDateTime::currentDateTime());
}

QMap<QString, QTBPage *> QTBProject::pages() const
{
    return mPages;
}


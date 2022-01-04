#ifndef BOARD_PROJECT_H
#define BOARD_PROJECT_H

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QFileSystemWatcher>
#include "page.h"
#include "alarms.h"
#include "ui/ui_global.h"
#include "ui/board/board_parameter.h"

#define PRO_SETTINGS_KEY_NAME "Name"
#define PRO_SETTINGS_KEY_CREATIONDATE "CreationDate"
#define PRO_SETTINGS_KEY_MODIFICATIONDATE "ModificationDate"

struct ElementDecl {
    QString page_name;
    QString parameter;
    QString type_name;
    int column;
    int row;
};

class BOARD_UI_EXPORT Project : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    Project(QObject *parent = nullptr);
    ~Project();

    bool generate(const QString& workingDirectory, const QString& projectName);
    bool load(const QString& workingDirectory, const QString& projectName, bool fullLoad = true);
    void clear();

    void fullReload();

    void loadPages();
    bool addPage(QExplicitlySharedDataPointer<Page> page);
    QExplicitlySharedDataPointer<Page> page(const QString& pageName);
    QExplicitlySharedDataPointer<Page> currentPage();
    QExplicitlySharedDataPointer<Page> requestedPage();
    QMap<QString, QExplicitlySharedDataPointer<Page>> pages() const;

    bool exportToCsv(QString filePath);

    void loadParametersSettings();
    void addParameterSettings(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings(const QString& name, const QString& description);
    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<ParameterConfiguration> > > parametersSettings() const;    

    void loadAlarmsConfigurations();
    void addAlarmConfiguration(QExplicitlySharedDataPointer<Alarms> alarmConfig);
    QMap<QString, QExplicitlySharedDataPointer<Alarms> > alarmsConfigurations() const;
    QExplicitlySharedDataPointer<Alarms> alarmConfiguration(const QString& name);

    //getter
    QDateTime creationDate() const;
    QDateTime modificationDate() const;
    QString name() const;

    //setter
    void setCreationDate(const QDateTime &creationDate);
    void setModificationDate(const QDateTime &modificationDate);
    void setName(const QString &name);

    QString alarmsConfigPath() const;
    QString paramSettingsPath() const;
    QString currentPageName() const;
    QString pagesPath() const;

    void setCurrentPageName(const QString &currentPageName);

    QString requestedPageName() const;
    void setRequestedPageName(const QString &requestedPageName);

    QList<ElementDecl> findParameter(QString parameterName);

    QString resourcesImagesPath() const;

    QString resourcesPath() const;

public slots:
    void requestPage(const QString& pageName);
//    void alarmFilesChanged(QString path);

signals:
    void loaded();
    void pagesListUpdated();
    void pageRequested();
    void pageLoaded();
    void updated();

private:
    QDateTime mCreationDate;
    QDateTime mModificationDate;
    QString  mName;
    QString mProjectPath;
    QString mSettingsPath;

    QString mCurrentPageName;
    QString mRequestedPageName;

    QString mResourcesPath;
    QString mResourcesImagesPath;

    QString mPagesPath;
    bool mPagesDirWrittable;

    QString mAlarmsConfigPath;
    QString mParamSettingsPath;
    bool mParamSettingsDirWrittable;

    QMap<QString, QExplicitlySharedDataPointer<Page>> mPages;
    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<ParameterConfiguration>>> mParametersSettings;
    QMap<QString, QExplicitlySharedDataPointer<Alarms>> mAlarmsConfigurations;

//    QFileSystemWatcher *mAlarmsWatcher;
};

#endif // BOARD_PROJECT_H

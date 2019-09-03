#ifndef QTBPROJECT_H
#define QTBPROJECT_H

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include "project/page.h"
#include "dashboard/dashboard_parameter.h"

#define PRO_SETTINGS_KEY_NAME "Name"
#define PRO_SETTINGS_KEY_CREATIONDATE "CreationDate"
#define PRO_SETTINGS_KEY_MODIFICATIONDATE "ModificationDate"

class QTBProject : public QObject
{
    Q_OBJECT
public:
    QTBProject(QObject *parent = nullptr);
    ~QTBProject();

    bool generate(const QString& workingDirectory, const QString& projectName);
    bool load(const QString& workingDirectory, const QString& projectName);
    void clear();

    void loadPages();
    bool addPage(QTBPage *page);
    QTBPage *page(const QString& pageName);
    QTBPage *currentPage();
    QTBPage *requestedPage();
    QMap<QString, QTBPage *> pages() const;

    void loadParametersSettings();
    void addParameterSettings(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);
    QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings(const QString& name, const QString& description);
    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration> > > parametersSettings() const;

    //getter
    QDateTime creationDate() const;
    QDateTime modificationDate() const;
    QString name() const;

    //setter
    void setCreationDate(const QDateTime &creationDate);
    void setModificationDate(const QDateTime &modificationDate);
    void setName(const QString &name);

    QString paramSettingsPath() const;
    QString currentPageName() const;
    QString pagesPath() const;

    void setCurrentPageName(const QString &currentPageName);

    QString requestedPageName() const;
    void setRequestedPageName(const QString &requestedPageName);

public slots:
    void requestPage(const QString& pageName);

signals:
    void loaded();
    void pagesListUpdated();
    void pageRequested();
    void pageLoaded();

private:
    QDateTime mCreationDate;
    QDateTime mModificationDate;
    QString  mName;
    QString mProjectPath;
    QString mSettingsPath;

    QString mCurrentPageName;
    QString mRequestedPageName;

    QString mPagesPath;
    bool mPagesDirWrittable;

    QString mParamSettingsPath;
    bool mParamSettingsDirWrittable;

    QMap<QString, QTBPage*> mPages;
    QMap<QString, QMap<QString, QExplicitlySharedDataPointer<QTBParameterConfiguration>>> mParametersSettings;

};

#endif // QTBPROJECT_H

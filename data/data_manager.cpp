#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QLibraryInfo>
#include "data_manager.h"
#include "live_data_source_interface.h"
#include "playback_data_source_interface.h"

DataManager::DataManager(QObject *parent): QObject(parent),
    mUseDataSourceAsReferenceClock(true),
    mIndexParameters(0),
    mLiveDataEnabled(true),
    mPlaybackStartTime(86399900),
    mPlaybackEndTime(0),
    mPlaybackStartValid(false),
    mPlaybackEndValid(false)
{
    mParametersTimer.setSingleShot(true);
    mParametersTimer.setInterval(TEMPO_MS_PARAM_UPDATE);
    connect(&mParametersTimer, SIGNAL(timeout()), this, SIGNAL(parametersUpdated()));
    connect(this, SIGNAL(startParametersTimer()), &mParametersTimer, SLOT(start()));

    QSettings settings(QCoreApplication::applicationDirPath() + QDir::separator() + QString("data_sources.profiles"),
                       QSettings::IniFormat);
    int profilesCount = settings.beginReadArray("Profiles");
    for(int i=0;i<profilesCount;i++) {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        QString long_name = settings.value("long_name").toString();
        QString working_directory = settings.value("working_directory").toString();

        if(!name.isEmpty()) {
            mProfiles.insert(name, long_name);
            mProfilesWorkingDirectory.insert(name,working_directory);
        }
    }
    settings.endArray();
}

DataManager::~DataManager()
{
    clear();
}

void DataManager::clear() {

    for(auto src: mLiveDataSources) {
        src->stop();
    }

    mLiveDataSources.clear();
    mPlaybackDataSources.clear();

    mLiveParametersById.clear();
}

QVector<QDate> DataManager::playbackValidDates() const
{
    return mPlaybackValidDates;
}

QMap<QString, QString> DataManager::profiles() const
{
    return mProfiles;
}

QString DataManager::profileName() const
{
    return mProfileName;
}

void DataManager::setProfileName(const QString &profileName, bool reload)
{
    if(profileName != mProfileName) {
        mProfileName = profileName;
        if(reload)
            loadDataSources();
    }
}

bool DataManager::registerLiveParameter(const QSharedPointer<TimeSeries> &param)
{
    if(param && param->valid() &&
            !param->name().isEmpty() && param->depth() > 0) {
        QMutexLocker locker(&mMutex);
        if(!mLiveParametersIdByLabels.contains(param->name())) {

            if(mPlaybackParametersIdByLabels.contains(param->name())) {
                param->setParameterId(mPlaybackParametersIdByLabels.value(param->name()));
            } else {
                mIndexParameters ++;
                param->setParameterId(mIndexParameters);
            }

            mLiveParametersById.insert(param->parameterId(), param);
            mLiveParametersIdByLabels.insert(param->name(), param->parameterId());

            emit startParametersTimer();
            return true;
        } else {
            logger()->warn() << "Live parameter already registered " << param->name();
        }
    }

    return false;
}

void DataManager::unregisterLiveParameter(const QSharedPointer<TimeSeries> &param)
{
    if(param) {
        QMutexLocker locker(&mMutex);
        if(mLiveParametersById.contains(param->parameterId())) {

            mLiveParametersById.remove(param->parameterId());
            mLiveParametersIdByLabels.remove(param->name());

            emit startParametersTimer();
        }
    }
}

void DataManager::unregisterLiveParameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    if(mLiveParametersById.contains(parameterId)) {
        QString label = mLiveParametersById.value(parameterId)->name();

        mLiveParametersById.remove(parameterId);
        mLiveParametersIdByLabels.remove(label);

        emit startParametersTimer();
    }
}

void DataManager::unregisterLiveParameter(const QString &label)
{
    QMutexLocker locker(&mMutex);
    if(mLiveParametersIdByLabels.contains(label)) {
        quint32 parameterId = mLiveParametersIdByLabels.value(label);
        if(mLiveParametersById.contains(parameterId)) {
            QString label = mLiveParametersById.value(parameterId)->name();

            mLiveParametersById.remove(parameterId);
            mLiveParametersIdByLabels.remove(label);

            emit startParametersTimer();
        }
    }
}

QSharedPointer<TimeSeries> DataManager::liveParameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<TimeSeries> param;
    if(mLiveParametersById.contains(parameterId)) {
        return mLiveParametersById.value(parameterId);
    }
    return param;
}

QSharedPointer<TimeSeries> DataManager::liveParameter(const QString &label)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<TimeSeries> param;
    if(mLiveParametersIdByLabels.contains(label)) {
        quint32 parameterId = mLiveParametersIdByLabels.value(label);
        if(mLiveParametersById.contains(parameterId)) {
            return mLiveParametersById.value(parameterId);
        }
    }
    return param;
}

bool DataManager::registerPlaybackParameter(const QSharedPointer<TimeSeries> &param)
{
    if(param && param->valid() &&
            !param->name().isEmpty() && param->depth() > 0) {
        QMutexLocker locker(&mMutex);

        if(!mPlaybackParametersIdByLabels.contains(param->name())) {

            if(mLiveParametersIdByLabels.contains(param->name())) {
                param->setParameterId(mLiveParametersIdByLabels.value(param->name()));
            } else {
                mIndexParameters ++;
                param->setParameterId(mIndexParameters);
            }

            mPlaybackParametersById.insert(param->parameterId(), param);
            mPlaybackParametersIdByLabels.insert(param->name(), param->parameterId());
            mPlaybackParameterSourceNames.insert(param->name(), param->sourceName());

            emit startParametersTimer();
            return true;
        }
    }

    return false;
}

void DataManager::unregisterPlaybackParameter(const QSharedPointer<TimeSeries> &param)
{
    if(param) {
        QMutexLocker locker(&mMutex);
        if(mPlaybackParametersById.contains(param->parameterId())) {

            mPlaybackParametersById.remove(param->parameterId());
            mPlaybackParametersIdByLabels.remove(param->name());
            mPlaybackParameterSourceNames.remove(param->name());

            emit startParametersTimer();
        }
    }
}

void DataManager::unregisterPlaybackParameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    if(mPlaybackParametersById.contains(parameterId)) {
        QString label = mPlaybackParametersById.value(parameterId)->name();

        mPlaybackParametersById.remove(parameterId);
        mPlaybackParametersIdByLabels.remove(label);
        mPlaybackParameterSourceNames.remove(label);

        emit startParametersTimer();
    }
}

void DataManager::unregisterPlaybackParameter(const QString &label)
{
    QMutexLocker locker(&mMutex);
    if(mPlaybackParametersIdByLabels.contains(label)) {
        quint32 parameterId = mPlaybackParametersIdByLabels.value(label);
        if(mPlaybackParametersById.contains(parameterId)) {
            QString label = mPlaybackParametersById.value(parameterId)->name();

            //            mPlaybackData.remove(parameterId);
            mPlaybackParametersById.remove(parameterId);
            mPlaybackParametersIdByLabels.remove(label);
            mPlaybackParameterSourceNames.remove(label);

            emit startParametersTimer();
        }
    }
}

QSharedPointer<TimeSeries> DataManager::playbackParameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<TimeSeries> param;
    if(mPlaybackParametersById.contains(parameterId)) {
        return mPlaybackParametersById.value(parameterId);
    }
    return param;
}

QSharedPointer<TimeSeries> DataManager::playbackParameter(const QString &label)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<TimeSeries> param;
    if(mPlaybackParametersIdByLabels.contains(label)) {
        quint32 parameterId = mPlaybackParametersIdByLabels.value(label);
        if(mPlaybackParametersById.contains(parameterId)) {
            return mPlaybackParametersById.value(parameterId);
        }
    }
    return param;
}

QSharedPointer<TimeSeries> DataManager::dataParameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<TimeSeries> param;
    if(mLiveDataEnabled) {
        if(mLiveParametersById.contains(parameterId)) {
            return mLiveParametersById.value(parameterId);
        }
    } else {
        if(mPlaybackParametersById.contains(parameterId)) {
            return mPlaybackParametersById.value(parameterId);
        }
    }
    return param;
}

QSharedPointer<TimeSeries> DataManager::dataParameter(const QString &label)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<TimeSeries> param;
    if(mLiveDataEnabled) {
        if(mLiveParametersIdByLabels.contains(label)) {
            quint32 parameterId = mLiveParametersIdByLabels.value(label);
            if(mLiveParametersById.contains(parameterId)) {
                return mLiveParametersById.value(parameterId);
            }
        }
    } else {
        if(mPlaybackParametersIdByLabels.contains(label)) {
            quint32 parameterId = mPlaybackParametersIdByLabels.value(label);
            if(mPlaybackParametersById.contains(parameterId)) {
                return mPlaybackParametersById.value(parameterId);
            }
        }
    }
    return param;
}

void DataManager::registerBoardParameterId(quint32 paramId)
{
    if(paramId > 0) {
        if(mBoardParametersCount.contains(paramId)) {
            mBoardParametersCount[paramId]++;
        } else {
            mBoardParametersCount.insert(paramId, 1);
            mBoardParameterIds.append(paramId);

            mLiveTimestamps.append(QVector<double>());
            mLiveValues.append(QVector<DataValue>());

            //            updateLiveBoardData(paramId);
        }
    }
}

void DataManager::unregisterBoardParameterId(quint32 paramId)
{    
    if(paramId > 0) {
        if(mBoardParameterIds.contains(paramId)) {
            mBoardParametersCount[paramId]--;
            if(mBoardParametersCount.value(paramId) == 0) {
                mBoardParametersCount.remove(paramId);
                int pos = mBoardParameterIds.indexOf(paramId);
                mBoardParameterIds.removeAt(pos);

                mLiveTimestamps.removeAt(pos);
                mLiveValues.removeAt(pos);
            }
        }
    }
}

void DataManager::liveReferenceTime(QDateTime &refTime)
{
    bool ok = false;
    if(mUseDataSourceAsReferenceClock) {
        if(mReferenceTimeLiveDataSource) {
            if(mReferenceTimeLiveDataSource->isTimeReference()) {
                refTime = mReferenceTimeLiveDataSource->referenceTime();
                ok = true;
            }
        }
    }

    if(!ok)
        refTime = QDateTime::currentDateTimeUtc();
}

void DataManager::updateLiveBoardData()
{
    for (int i=0; i<mBoardParameterIds.size(); ++i) {
        if(mLiveParametersById.contains(mBoardParameterIds.at(i))) {

            QVector<DataValue> values;
            QVector<double> timestamps;

            mLiveParametersById[mBoardParameterIds.at(i)]->read(true, timestamps, values);

            mLiveTimestamps[i].resize(timestamps.count());
            mLiveValues[i].resize(values.count());

            QVector<DataValue>::const_iterator itValues;
            QVector<double>::const_iterator itTimestamps;

            int index = 0;
            for (itTimestamps = timestamps.constBegin();
                 itTimestamps != timestamps.constEnd();
                 ++itTimestamps) {

                mLiveTimestamps[i][index] = *itTimestamps;
                index ++;
            }
            index = 0;
            for (itValues = values.constBegin();
                 (itValues != values.constEnd());
                 ++itValues) {

                mLiveValues[i][index] = *itValues;
                index ++;
            }
        }
    }
}

void DataManager::loadLiveBoardData()
{
    for (int i=0; i<mBoardParameterIds.size(); ++i) {
        if(mLiveParametersById.contains(mBoardParameterIds.at(i))) {

            QVector<DataValue> values;
            QVector<double> timestamps;

            mLiveParametersById[mBoardParameterIds.at(i)]->read(false, timestamps, values);

            mLiveTimestamps[i].resize(timestamps.count());
            mLiveValues[i].resize(values.count());

            QVector<DataValue>::const_iterator itValues;
            QVector<double>::const_iterator itTimestamps;

            int index = 0;
            for (itTimestamps = timestamps.constBegin();
                 itTimestamps != timestamps.constEnd();
                 ++itTimestamps) {

                mLiveTimestamps[i][index] = *itTimestamps;
                index ++;
            }
            index = 0;
            for (itValues = values.constBegin();
                 (itValues != values.constEnd());
                 ++itValues) {

                mLiveValues[i][index] = *itValues;
                index ++;
            }
        }
    }
}

//void DataManager::updateLiveBoardData(quint32 paramId)
//{
//    int pos = mBoardParameterIds.indexOf(paramId);
//    if(pos >= 0 && mLiveParametersById.contains(paramId)) {

//        QVector<DataValue> values;
//        QVector<double> timestamps;

//        mLiveParametersById[paramId]->read(false,timestamps, values);

//        mLiveTimestamps[pos].resize(timestamps.count());
//        mLiveValues[pos].resize(values.count());

//        QVector<DataValue>::const_iterator itValues;
//        QVector<double>::const_iterator itTimestamps;

//        int index = 0;
//        for (itTimestamps = timestamps.constBegin();
//             itTimestamps != timestamps.constEnd();
//             ++itTimestamps) {

//            mLiveTimestamps[pos][index] = *itTimestamps;
//            index ++;
//        }
//        index = 0;
//        for (itValues = values.constBegin();
//             (itValues != values.constEnd());
//             ++itValues) {

//            mLiveValues[pos][index] = *itValues;
//            index ++;
//        }
//    }
//}

void DataManager::reinitPlayback()
{    
    clearPlaybackBoardData();
    mPlaybackStartTime = 86399900;
    mPlaybackEndTime = 0;
    mPlaybackStartValid = false;
    mPlaybackEndValid = false;
    emit playbackTimeUpdated();
}

QHash<quint32, QSharedPointer<TimeSeries> > DataManager::playbackParametersById() const
{
    return mPlaybackParametersById;
}

QHash<quint32, QSharedPointer<TimeSeries> > DataManager::liveParametersById() const
{
    return mLiveParametersById;
}


bool DataManager::setPlaybackDateTime(double secsSinceStartOfTheDay, QDate date)
{
    bool reloadParam = false;
    mPlaybackTimestampSinceStartOfTheDay = secsSinceStartOfTheDay;
    for(auto src: mPlaybackDataSources) {
        src->setPlaybackDateTime(mPlaybackTimestampSinceStartOfTheDay, date);
        if(src->parametersUpdated())
            reloadParam = true;
    }

    return reloadParam;
}

void DataManager::updatePlaybackBoardData()
{
    QVector<QSharedPointer<TimeSeries>> boardParameters;
    for (auto paramId : mBoardParameterIds) {
        boardParameters.append(mPlaybackParametersById.value(paramId));
    }

    for(auto src: mPlaybackDataSources) {
        src->loadPlaybackData(boardParameters,
                              mPlaybackTimestamps,
                              mPlaybackValues);
    }
}

void DataManager::clearPlaybackBoardData()
{
    mPlaybackValues.clear();
    mPlaybackTimestamps.clear();
}

bool DataManager::lastBoardData(quint32 paramId, QVector<DataValue> &values, QVector<double> &timestamps)
{
    if(mLiveDataEnabled) {
        int pos = mBoardParameterIds.indexOf(paramId);
        if(pos >= 0) {
            values = mLiveValues[pos];
            timestamps = mLiveTimestamps[pos];

            return true;
        }
    } else {
        if(mPlaybackTimestamps.contains(paramId) &&
                mPlaybackValues.contains(paramId) ) {

            values = mPlaybackValues.value(paramId);
            timestamps = mPlaybackTimestamps.value(paramId);
            return true;
        }
    }
    return false;
}

bool DataManager::lastBoardData(quint32 paramId, DataValue &value, double &timestamp)
{
    if(mLiveDataEnabled) {
        int pos = mBoardParameterIds.indexOf(paramId);
        if(pos >= 0) {
            if(mLiveValues[pos].size() > 0 &&
                    mLiveTimestamps[pos].size()) {

                value = mLiveValues[pos].last();
                timestamp = mLiveTimestamps[pos].last();

                return true;
            }
        }
    }  else {
        if(mPlaybackTimestamps.contains(paramId) &&
                mPlaybackValues.contains(paramId) ) {

            auto values = mPlaybackValues.value(paramId);
            auto timestamps = mPlaybackTimestamps.value(paramId);

            auto itTimestamps = std::lower_bound(timestamps.begin(), timestamps.end(), mPlaybackTimestampSinceStartOfTheDay);

            if(itTimestamps != timestamps.end()) {
                if(itTimestamps != timestamps.begin())
                    itTimestamps--;

                auto pos = itTimestamps - timestamps.begin();
                timestamp = timestamps.at(int(pos));
                value = values.at(int(pos));

                return true;
            }
        }
    }
    return false;
}

void DataManager::loadDataSources()
{
    clear();

    QSettings settings(QCoreApplication::applicationDirPath() + QDir::separator() + QString("data_sources.profiles"),
                       QSettings::IniFormat);

    int profilesCount = settings.beginReadArray("Profiles");

    if(mProfileName.isEmpty())
        return;

    QString liveSourceName, playbackSourceName;
    QMap<QString, QString> liveProperties, playbackProperties;

    for(int i=0;i<profilesCount;i++) {

        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();

        if(name == mProfileName) {

            settings.beginGroup("Live");
            liveSourceName = settings.value("name").toString();

            settings.beginGroup("Properties");
            auto livePropList = settings.childKeys();
            for(auto prop : livePropList) {
                liveProperties.insert(prop, settings.value(prop).toString());
            }
            settings.endGroup();

            settings.endGroup();


            settings.beginGroup("Playback");
            playbackSourceName = settings.value("name").toString();

            settings.beginGroup("Properties");
            auto playbackPropList = settings.childKeys();
            for(auto prop : playbackPropList) {
                playbackProperties.insert(prop, settings.value(prop).toString());
            }
            settings.endGroup();

            settings.endGroup();
            break;
        }
    }

    settings.endArray();

    QStringList pathList;
    pathList << QCoreApplication::applicationDirPath() << QLibraryInfo::location(QLibraryInfo::LibrariesPath);

    for(auto libPath : pathList) {
        QDir pluginsDir(libPath);
        QDir dataSrcDir(pluginsDir.absoluteFilePath(libPath));

        QStringList filter;
#ifdef __linux__
        filter << "libdatasrc*.so";
#elif _WIN32
        filter << "datasrc*.dll";
#endif
        dataSrcDir.setNameFilters(filter);

        for (const QString& filename: dataSrcDir.entryList(QDir::Files | QDir::Readable)) {
            QFileInfo fileinfo(filename);
            logger()->info() << "Loading data source " << filename;

            QPluginLoader pluginLoader(dataSrcDir.absoluteFilePath(filename), this);

            QObject *plugin = pluginLoader.instance();
            if (plugin) {
                QSharedPointer<LiveDataSourceInterface> dataSourceLive = QSharedPointer<LiveDataSourceInterface>(qobject_cast<LiveDataSourceInterface *>(plugin));
                if(dataSourceLive) {
                    if(dataSourceLive->name() == liveSourceName) {
                        logger()->info() << "Activating live data source " << filename;
                        dataSourceLive->setDataManager(this);

                        for(auto it = liveProperties.begin(); it != liveProperties.end(); ++it) {
                            dataSourceLive->setProperty(it.key().toStdString().c_str(), it.value());
                        }

                        mLiveDataSources.insert(dataSourceLive->name(), dataSourceLive);
                        if(mReferenceTimeLiveDataSource.isNull())
                            if(dataSourceLive->isTimeReference())
                                mReferenceTimeLiveDataSource = dataSourceLive;
                        dataSourceLive->start();
                    }
                }

                QSharedPointer<PlaybackDataSourceInterface> dataSourcePlayback = QSharedPointer<PlaybackDataSourceInterface>(qobject_cast<PlaybackDataSourceInterface *>(plugin));
                if(dataSourcePlayback) {
                    if(dataSourcePlayback->name() == playbackSourceName) {
                        logger()->info() << "Activating playback data source " << filename;
                        dataSourcePlayback->setDataManager(this);

                        for(auto it = playbackProperties.begin(); it != playbackProperties.end(); ++it) {
                            dataSourcePlayback->setProperty(it.key().toStdString().c_str(), it.value());
                        }

                        mPlaybackDataSources.insert(dataSourcePlayback->name(), dataSourcePlayback);
                        dataSourcePlayback->activatePlayback();
                    }
                }

            } else {
                logger()->debug() << filename << " " << pluginLoader.errorString();
            }
        }
    }
}

QMap<QString, QSharedPointer<PlaybackDataSourceInterface> > DataManager::playbackDataSources() const
{
    return mPlaybackDataSources;
}

QMap<QString, QSharedPointer<LiveDataSourceInterface> > DataManager::liveDataSources() const
{
    return mLiveDataSources;
}

int DataManager::playbackEndTime(bool &ok) const
{
    ok = mPlaybackEndValid;
    return mPlaybackEndTime;
}

int DataManager::playbackStartTime(bool &ok) const
{
    ok = mPlaybackStartValid;
    return mPlaybackStartTime;
}

bool DataManager::liveDataEnabled() const
{
    return mLiveDataEnabled;
}

void DataManager::liveDataEnabled(bool live)
{
    mLiveDataEnabled = live;
}

void DataManager::newPlaybackStartTime(int msecs)
{
    if(msecs < mPlaybackStartTime) {
        mPlaybackStartValid = true;
        mPlaybackStartTime = msecs;
        emit playbackTimeUpdated();
    }
}

void DataManager::newPlaybackEndTime(int msecs)
{
    if(msecs > mPlaybackEndTime) {
        mPlaybackEndValid = true;
        mPlaybackEndTime = msecs;
        emit playbackTimeUpdated();
    }
}

void DataManager::newPlaybackValidDate(QDate date)
{
    if(!mPlaybackValidDates.contains(date)) {
        mPlaybackValidDates.append(date);
        emit playbackValidDatesUpdated();
    }
}

void DataManager::newPlaybackValidDates(QVector<QDate> dates)
{
    bool update = false;
    for(auto date : dates) {
        if(!mPlaybackValidDates.contains(date)) {
            mPlaybackValidDates.append(date);
            update=true;
        }
    }
    if(update)
        emit playbackValidDatesUpdated();
}

bool DataManager::useDataSourceAsReferenceClock() const
{
    return mUseDataSourceAsReferenceClock;
}

void DataManager::setUseDataSourceAsReferenceClock(bool useDataSourceAsReferenceClock)
{
    mUseDataSourceAsReferenceClock = useDataSourceAsReferenceClock;
}

QString DataManager::profileWorkingDirectory()
{
    QString wDir;
    if(!mProfileName.isEmpty()) {
        if(mProfilesWorkingDirectory.contains(mProfileName))
            wDir = mProfilesWorkingDirectory.value(mProfileName);
    }
    return wDir;
}

void DataManager::setReferenceTimeLiveDataSource(const QSharedPointer<LiveDataSourceInterface> &referenceTimeLiveDataSource)
{
    mReferenceTimeLiveDataSource = referenceTimeLiveDataSource;
}

QSharedPointer<LiveDataSourceInterface> DataManager::referenceTimeLiveDataSource() const
{
    return mReferenceTimeLiveDataSource;
}

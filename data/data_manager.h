#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "data_global.h"
#include "time_series.h"
#include "log4qt/logger.h"

#define TEMPO_MS_PARAM_UPDATE 200

class DataSource;
class LiveDataSourceInterface;
class PlaybackDataSourceInterface;
class DATASHARED_EXPORT DataManager: public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    DataManager(QObject *parent = nullptr);
    ~DataManager();

    bool registerLiveParameter(const QSharedPointer<TimeSeries>& param);
    void unregisterLiveParameter(const QSharedPointer<TimeSeries>& param);
    void unregisterLiveParameter(quint32 parameterId);
    void unregisterLiveParameter(const QString& label);
    QSharedPointer<TimeSeries> liveParameter(quint32 parameterId);
    QSharedPointer<TimeSeries> liveParameter(const QString& label);
    QHash<quint32, QSharedPointer<TimeSeries> > liveParametersById() const;

    void liveReferenceTime(QDateTime &refTime);
    bool liveDataEnabled() const;
    void liveDataEnabled(bool liveDataEnabled);

    bool registerPlaybackParameter(const QSharedPointer<TimeSeries>& param);
    void unregisterPlaybackParameter(const QSharedPointer<TimeSeries>& param);
    void unregisterPlaybackParameter(quint32 parameterId);
    void unregisterPlaybackParameter(const QString& label);
    QSharedPointer<TimeSeries> playbackParameter(quint32 parameterId);
    QSharedPointer<TimeSeries> playbackParameter(const QString& label);
    QHash<quint32, QSharedPointer<TimeSeries> > playbackParametersById() const;

    int playbackStartTime(bool &ok) const;
    int playbackEndTime(bool &ok) const;
    void newPlaybackValidDate(QDate date);
    void newPlaybackValidDates(QVector<QDate> dates);

    QSharedPointer<TimeSeries> dataParameter(quint32 parameterId);
    QSharedPointer<TimeSeries> dataParameter(const QString& label);

    void registerBoardParameterId(quint32 paramId);
    void unregisterBoardParameterId(quint32 paramId);    
    void updateLiveBoardData();
    void loadLiveBoardData();
    bool setPlaybackDateTime(double secsSinceStartOfTheDay, QDate date);
    void updatePlaybackBoardData();
    void clearPlaybackBoardData();
    bool lastBoardData(quint32 paramId, QVector<DataValue> &values, QVector<double> &timestamps);
    bool lastBoardData(quint32 paramId, DataValue &value, double &timestamp);

    void loadDataSources();
    QMap<QString, QSharedPointer<DataSource>> dataSources() const;
    QMap<QString, QSharedPointer<LiveDataSourceInterface> > liveDataSources() const;
    QMap<QString, QSharedPointer<PlaybackDataSourceInterface> > playbackDataSources() const;
    QSharedPointer<LiveDataSourceInterface> referenceTimeLiveDataSource() const;
    void setReferenceTimeLiveDataSource(const QSharedPointer<LiveDataSourceInterface> &referenceTimeLiveDataSource);
    bool useDataSourceAsReferenceClock() const;
    void setUseDataSourceAsReferenceClock(bool useDataSourceAsReferenceClock);

    QString profileWorkingDirectory();
    QString profileName() const;
    void setProfileName(const QString &profileName, bool reload = false);

    QMap<QString, QString> profiles() const;

    QVector<QDate> playbackValidDates() const;

signals:
    void parametersUpdated();
    void startParametersTimer();
    void playbackTimeUpdated();
    void playbackValidDatesUpdated();

public slots:
    void newPlaybackStartTime(int msecs);
    void newPlaybackEndTime(int msecs);
    void reinitPlayback();

protected:
    void clear();

private:
    QHash<quint32, QSharedPointer<TimeSeries>> mLiveParametersById;
    QHash<QString, quint32> mLiveParametersIdByLabels;

    QHash<quint32, QSharedPointer<TimeSeries>> mPlaybackParametersById;
    QHash<QString, quint32> mPlaybackParametersIdByLabels;
    QHash<QString, QString> mPlaybackParameterSourceNames;

    QMap<QString, QSharedPointer<LiveDataSourceInterface>> mLiveDataSources;
    QMap<QString, QSharedPointer<PlaybackDataSourceInterface>> mPlaybackDataSources;
    QSharedPointer<LiveDataSourceInterface> mReferenceTimeLiveDataSource;
    bool mUseDataSourceAsReferenceClock;

    quint32 mIndexParameters;

    QMap<quint32, int> mBoardParametersCount;
    QVector<quint32> mBoardParameterIds;    

    QVector<QVector<double>> mLiveTimestamps;
    QVector<QVector<DataValue>> mLiveValues;

    QMap<quint32, QVector<double>> mPlaybackTimestamps;
    QMap<quint32, QVector<DataValue>> mPlaybackValues;

    double mPlaybackTimestampSinceStartOfTheDay;
    QVector<QDate> mPlaybackValidDates;

    bool mLiveDataEnabled;
    int mPlaybackStartTime;
    int mPlaybackEndTime;
    bool mPlaybackStartValid;
    bool mPlaybackEndValid;

    QTimer mParametersTimer;
    mutable QMutex mMutex;

    QString mProfileName;
    QMap<QString, QString> mProfiles;
    QMap<QString, QString> mProfilesWorkingDirectory;

};

#endif // DATA_MANAGER_H

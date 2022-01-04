#ifndef PLAYBACK_DATA_SOURCE_INTERFACE_H
#define PLAYBACK_DATA_SOURCE_INTERFACE_H

#include <QDate>
#include "data_source.h"
#include "data_global.h"
#include "log4qt/logger.h"

class DATASHARED_EXPORT PlaybackDataSourceInterface : public DataSource
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:

    PlaybackDataSourceInterface(QString name) :
        DataSource(name)
    {

    }
    virtual ~PlaybackDataSourceInterface() override {}

    virtual bool activatePlayback() = 0;
    virtual void setPlaybackDateTime(double secsSinceStartOfTheDay, QDate date = QDate()) = 0;
    virtual void loadPlaybackData(QVector<QSharedPointer<TimeSeries>> boardParameters,
                                  QMap<quint32, QVector<double>> &timestamps,
                                  QMap<quint32, QVector<DataValue>> &values) = 0;

    virtual bool parametersUpdated() = 0;
    void clearParameters() override
    {
        if(mDataManager) {
            foreach(auto param, mListParam)
                mDataManager->unregisterPlaybackParameter(param);
            mListParam.clear();
        }
    }

    void unregisterParameter(const QString& label) override
    {
        if(mDataManager) {
            QSharedPointer<TimeSeries> param;
            param = mDataManager->playbackParameter(label);
            mDataManager->unregisterPlaybackParameter(label);
            if(param)
                mListParam.removeAll(param);
        }
    }

    void unregisterParameter(quint32 parameterId) override
    {
        if(mDataManager) {
            QSharedPointer<TimeSeries> param;
            param = mDataManager->playbackParameter(parameterId);
            mDataManager->unregisterPlaybackParameter(parameterId);
            if(param)
                mListParam.removeAll(param);
        }
    }

    virtual bool registerParameter(const QSharedPointer<TimeSeries>& param) override
    {
        bool st = false;
        if(mDataManager) {
            if(param->valid()) {
                st = mDataManager->registerPlaybackParameter(param);
                if(st)
                    mListParam.append(param);
            } else {
                logger()->debug() << Q_FUNC_INFO << "Param is not valid - failed to allocate memory" << param->name() << param->capacity();
            }
        }

        return st;
    }


signals:
    void playbackStartTime(int);
    void playbackEndTime(int);

protected:
    virtual void dataManagerInitialized() override
    {
        connect(this, &PlaybackDataSourceInterface::playbackStartTime,
                mDataManager, &DataManager::newPlaybackStartTime);
        connect(this, &PlaybackDataSourceInterface::playbackEndTime,
                mDataManager, &DataManager::newPlaybackEndTime);
    }
};

#define PlaybackDataSourceInterface_iid "CuteBoard.PlaybackDataSources"

Q_DECLARE_INTERFACE(PlaybackDataSourceInterface, PlaybackDataSourceInterface_iid)

#endif // PLAYBACK_DATA_SOURCE_INTERFACE_H

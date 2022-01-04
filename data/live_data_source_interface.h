#ifndef LIVE_DATA_SOURCE_INTERFACE_H
#define LIVE_DATA_SOURCE_INTERFACE_H

#include <QDateTime>
#include "data_source.h"
#include "data_global.h"
#include "log4qt/logger.h"

class DATASHARED_EXPORT LiveDataSourceInterface : public DataSource
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:

    LiveDataSourceInterface(QString name) :
        DataSource(name),
        mIsTimeReference(false)
    {

    }

    virtual ~LiveDataSourceInterface() override
    {
        stop();
    }

    void start()
    {
        if(mStatus != dssRunning) {
            if(startLiveAcquisition()) {
                setStatus(dssRunning);
            } else {
                if(stopLiveAcquisition()) {
                    setStatus(dssIdle);
                } else {
                    setStatus(dssError);
                }
            }
        }
    }

    void stop()
    {
        if(mStatus == dssRunning) {
            if(stopLiveAcquisition()) {
                setStatus(dssIdle);
                clearParameters();
            } else {
                setStatus(dssError);
            }
        }
    }

    void clearParameters() override
    {
        if(mDataManager) {
            foreach(auto param, mListParam)
                mDataManager->unregisterLiveParameter(param);
            mListParam.clear();
        }
    }

    void unregisterParameter(const QString& label) override
    {
        if(mDataManager) {
            QSharedPointer<TimeSeries> param;
            param = mDataManager->liveParameter(label);
            mDataManager->unregisterLiveParameter(label);
            if(param)
                mListParam.removeAll(param);
        }
    }

    void unregisterParameter(quint32 parameterId) override
    {
        if(mDataManager) {
            QSharedPointer<TimeSeries> param;
            param = mDataManager->liveParameter(parameterId);
            mDataManager->unregisterLiveParameter(parameterId);
            if(param)
                mListParam.removeAll(param);
        }
    }

    virtual bool registerParameter(const QSharedPointer<TimeSeries>& param) override
    {
        bool st = false;
        if(mDataManager) {
            if(param->valid()) {
                st = mDataManager->registerLiveParameter(param);
                if(st)
                    mListParam.append(param);
            } else {
                logger()->debug() << Q_FUNC_INFO << "Param is not valid - failed to allocate memory" << param->name() << param->capacity();
            }
        }

        return st;
    }

    QDateTime referenceTime() const
    {
        return mReferenceTime;
    }

    bool isTimeReference() const
    {
        return mIsTimeReference;
    }

    void setReferenceTime(const QDateTime &referenceTime)
    {
        mReferenceTime = referenceTime;
    }

    void setIsTimeReference(bool isTimeReference)
    {
        mIsTimeReference = isTimeReference;
    }

protected:
    virtual bool startLiveAcquisition() = 0;
    virtual bool stopLiveAcquisition() = 0;
    QDateTime mReferenceTime;
    bool mIsTimeReference;

};

#define LiveDataSourceInterface_iid "CuteBoard.LiveDataSources"





Q_DECLARE_INTERFACE(LiveDataSourceInterface, LiveDataSourceInterface_iid)

#endif // LIVE_DATA_SOURCE_INTERFACE_H

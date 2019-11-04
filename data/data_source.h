#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include "data_manager.h"

class DataSource : public QObject
{
    Q_OBJECT
public:

    enum DataSourceStatus {
        dssIdle,
        dssRunning,
        dssError
    };
    Q_ENUM(DataSourceStatus)

    DataSource() : QObject(),
        mDataManager(nullptr),
        mStatus(dssIdle),
        mAutoStart(false) {}

    virtual ~DataSource() {}

    virtual bool startAcquisition() { return false; }
    virtual bool stopAcquisition() { return false; }

    bool autoStart() const
    {
        return mAutoStart;
    }

    void setAutoStart(bool autoStart)
    {
        mAutoStart = autoStart;
    }

    void updateSample(quint32 serieIndex, double timestamp, QTBDataValue value)
    {
        QMutexLocker locker(&mMutex);
        mData.insert(serieIndex, QTBDataSample(timestamp, value));
    }

    QString currentPath() { return mCurrentPath; }

    bool registerParameter(const QSharedPointer<QTBParameter>& param)
    {
        if(mDataManager)
            return mDataManager->registerParameter(param);
        else
            return false;
    }

    void unregisterParameter(quint32 parameterId)
    {
        if(mDataManager)
            mDataManager->unregisterParameter(parameterId);
    }

    void unregisterParameter(const QString& label)
    {
        if(mDataManager)
            mDataManager->unregisterParameter(label);
    }

signals:
    void statusChanged();
    void updateDashboard();

private:
    void setDataManager(QTBDataManager *dataManager)
    {
        mDataManager = dataManager;
    }

    void updateDashboardData()
    {
        if(mStatus == dssRunning) {
            QMutexLocker locker(&mMutex);
            QHash<quint32, QTBDataSample>::iterator i;
            for (i = mData.begin(); i != mData.end(); ++i) {
                mDataManager->addSampleUnsafe(i.key(),
                                              i.value().datationSec(),
                                              i.value().value());
            }
        }
    }

    void setStatus(const DataSourceStatus &status)
    {
        if(status != mStatus) {
            mStatus = status;
            emit statusChanged();
        }
    }

    DataSourceStatus status() {return mStatus; }

    void start() {
        if(mStatus != dssRunning) {
            if(startAcquisition()) {
                setStatus(dssRunning);
            } else {
                if(stopAcquisition()) {
                    setStatus(dssIdle);
                } else {
                    setStatus(dssError);
                }
            }
        }
    }

    void stop() {
        if(mStatus == dssRunning) {
            if(stopAcquisition()) {
                setStatus(dssIdle);
            } else {
                setStatus(dssError);
            }
        }
    }

    void setCurrentPath(const QString &currentPath) {
        mCurrentPath = currentPath;
    }

    QTBDataManager* mDataManager;
    QString mCurrentPath;
    DataSourceStatus mStatus;
    QMutex mMutex;
    QHash<quint32, QTBDataSample> mData;
    bool mAutoStart;

    friend class QTBDataManager;
    friend class QTBSettingsDialog;

};
#endif // DATA_SOURCE_H

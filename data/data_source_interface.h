#ifndef DATA_SOURCE_INTERFACE_H
#define DATA_SOURCE_INTERFACE_H

#include "data_manager.h"

class DataSourceInterface : public QObject
{
    Q_OBJECT
public:

    enum DataSourceStatus {
        dssInitialized,
        dssStarted,
        dssFinished,
        dssError
    };
    Q_ENUM(DataSourceStatus)

    DataSourceInterface() : QObject(), mDataManager(nullptr){}

    virtual ~DataSourceInterface() {}

    virtual void startAcquisition() = 0;
    virtual void stopAcquisition() = 0;

    DataSourceStatus status() const
    {
        return mStatus;
    }

    void setDataManager(QTBDataManager *dataManager)
    {
        mDataManager = dataManager;
    }

    void updateSample(quint32 serieIndex, double timestamp, QTBDataValue value)
    {
        QMutexLocker locker(&mMutex);
        mData.insert(serieIndex, QTBDataSample(timestamp, value));
    }

    void updateDashboardData()
    {
        if(mStatus == dssStarted) {
            QMutexLocker locker(&mMutex);
            QHash<quint32, QTBDataSample>::iterator i;
            for (i = mData.begin(); i != mData.end(); ++i) {
                mDataManager->addSampleUnsafe(i.key(),
                                              i.value().datationSec(),
                                              i.value().value());
            }
        }
    }

    QTBDataManager *dataManager() const {
        return mDataManager;
    }

    void setCurrentPath(const QString &currentPath) {
        mCurrentPath = currentPath;
    }

    QString currentPath() const
    {
        return mCurrentPath;
    }

    void setStatus(const DataSourceStatus &status)
    {
        if(status != mStatus) {
            mStatus = status;
            emit statusChanged();
        }
    }

    bool autoStart() const
    {
        return mAutoStart;
    }

    void setAutoStart(bool autoStart)
    {
        mAutoStart = autoStart;
    }

signals:
    void statusChanged();

protected:
    QTBDataManager* mDataManager;
    QString mCurrentPath;

private:
    QMutex mMutex;
    QHash<quint32, QTBDataSample> mData;
    DataSourceStatus mStatus;
    bool mAutoStart;

};

#define DataSourceInterface_iid "CuteBoard.DataSources"

Q_DECLARE_INTERFACE(DataSourceInterface, DataSourceInterface_iid)

#endif // DATA_SOURCE_INTERFACE_H

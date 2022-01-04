#ifndef DEMODATASOURCE_H
#define DEMODATASOURCE_H

#include <QtPlugin>
#include <QThread>
#include <QTimer>
#include "live_data_source_interface.h"
#include "demoparameter.h"

class DemoDataSource: public LiveDataSourceInterface
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_PLUGIN_METADATA(IID LiveDataSourceInterface_iid FILE "demodatasource.json")
    Q_INTERFACES(LiveDataSourceInterface)

public:
    DemoDataSource();
    ~DemoDataSource() override;

    bool startLiveAcquisition() override;
    bool stopLiveAcquisition() override;

    void loadParameters();
    void registerParameters();
    void unregisterParameters();

public slots:
    void updateData();

protected:
    QThread *mThread;
    QTimer *mTimer;
    QList<QSharedPointer<DemoParameter>> mListDemoParam;
    int mPeriodMillisec;
};

#endif // DEMODATASOURCE_H

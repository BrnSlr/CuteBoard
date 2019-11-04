#ifndef DEMODATASOURCE_H
#define DEMODATASOURCE_H

#include <QtPlugin>
#include <QThread>
#include <QTimer>
#include "data/data_source_interface.h"
#include "demoparameter.h"

class DemoDataSource: public DataSourceInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DataSourceInterface_iid FILE "demodatasource.json")
    Q_INTERFACES(DataSourceInterface)

public:
    DemoDataSource();
    ~DemoDataSource() override;

    bool startAcquisition() override;
    bool stopAcquisition() override;

    void loadParameters();
    void registerParameters();
    void unregisterParameters();

public slots:
    void updateData();

protected:
    QThread *mThread;
    QTimer *mTimer;
    QList<QSharedPointer<DemoParameter>> mListParam;
};

#endif // DEMODATASOURCE_H

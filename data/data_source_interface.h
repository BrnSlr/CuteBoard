#ifndef DATA_SOURCE_INTERFACE_H
#define DATA_SOURCE_INTERFACE_H

#include "data_source.h"

class DataSourceInterface : public DataSource
{
    Q_OBJECT
public:

    DataSourceInterface() : DataSource() {}
    virtual ~DataSourceInterface() {}

protected:
    virtual bool startAcquisition() = 0;
    virtual bool stopAcquisition() = 0;
};

#define DataSourceInterface_iid "CuteBoard.DataSources"

Q_DECLARE_INTERFACE(DataSourceInterface, DataSourceInterface_iid)

#endif // DATA_SOURCE_INTERFACE_H

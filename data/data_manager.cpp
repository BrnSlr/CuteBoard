#include "data_manager.h"
#include "data_source_interface.h"

QTBDataManager::QTBDataManager(QObject *parent) : QObject(parent)
{
    mDataBuffer = QSharedPointer<QTBDataBuffer>(new QTBDataBuffer());
    mParametersTimer = new QTimer(this);
    mParametersTimer->setSingleShot(true);
    mParametersTimer->setInterval(TEMPO_MS_PARAM_UPDATE);
    connect(mParametersTimer, SIGNAL(timeout()), this, SIGNAL(parametersUpdated()));
    loadDataSources();

    mThread = new QThread(this);
    mDataTimer = new QTimer(nullptr); // _not_ this!
    mDataTimer->setInterval(100);
    mDataTimer->setTimerType(Qt::PreciseTimer);
    mDataTimer->moveToThread(mThread);
    connect(mDataTimer, SIGNAL(timeout()), SLOT(updateData()), Qt::DirectConnection);
    connect(mThread, SIGNAL (finished()), mDataTimer, SLOT (deleteLater()));
    connect(mThread, SIGNAL (finished()), mThread, SLOT (deleteLater()));
    mDataTimer->connect(mThread, SIGNAL(started()), SLOT(start()));
    mThread->start();
}

QTBDataManager::~QTBDataManager()
{
    mThread->quit();
    mThread->wait();

    QMap<QString, DataSourceInterface *>::const_iterator iter_datasource;
    for (iter_datasource = mDataSources.constBegin(); iter_datasource != mDataSources.constEnd(); ++iter_datasource)
        iter_datasource.value()->stopAcquisition();

    qDeleteAll(mDataSources);
}

bool QTBDataManager::registerParameter(const QSharedPointer<QTBParameter>& param)
{
    QMutexLocker locker(&mMutex);
    if(!mParameterLabels.contains(param->label())) {
        quint32 parameterId = mDataBuffer->createSerie();
        param->setParameterId(parameterId);
        mParameters.insert(parameterId, param);
        mParameterLabels.insert(param->label(), parameterId);

        mParametersTimer->start();
        return true;
    }

    return false;
}

bool QTBDataManager::registerParameterUnsafe(const QSharedPointer<QTBParameter>& param)
{
    if(!mParameterLabels.contains(param->label())) {
        quint32 parameterId = mDataBuffer->createSerie();
        param->setParameterId(parameterId);
        mParameters.insert(parameterId, param);
        mParameterLabels.insert(param->label(), parameterId);

        mParametersTimer->start();
        return true;
    }

    return false;
}

QList<bool> QTBDataManager::registerParameters(const QList<QSharedPointer<QTBParameter> >& listParam)
{
    QMutexLocker locker(&mMutex);
    QList<bool> listStatus;

    if(listParam.count() > 0) {
        for(int i=0;i<listParam.count();i++) {
            bool st = registerParameterUnsafe(listParam.at(i));
            listStatus.append(st);
        }
        mParametersTimer->start();
    }

    return listStatus;
}

void QTBDataManager::unregisterParameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    if(mParameters.contains(parameterId)) {
        QString label = mParameters.value(parameterId)->label();
        mDataBuffer->removeSerie(parameterId);
        mParameters.remove(parameterId);
        mParameterLabels.remove(label);

        mParametersTimer->start();
    }
}

void QTBDataManager::unregisterParameter(const QString& label)
{
    QMutexLocker locker(&mMutex);
    if(mParameterLabels.contains(label)) {
        quint32 parameterId = mParameterLabels.value(label);
        if(mParameters.contains(parameterId)) {
            QString label = mParameters.value(parameterId)->label();
            mDataBuffer->removeSerie(parameterId);
            mParameters.remove(parameterId);
            mParameterLabels.remove(label);

            mParametersTimer->start();
        }
    }
}

QSharedPointer<QTBParameter> QTBDataManager::parameter(quint32 parameterId)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<QTBParameter> param = nullptr;
    if(mParameters.contains(parameterId)) {
        return mParameters.value(parameterId);
    }
    return param;
}

QSharedPointer<QTBParameter> QTBDataManager::parameter(const QString& label)
{
    QMutexLocker locker(&mMutex);
    QSharedPointer<QTBParameter> param = nullptr;
    if(mParameterLabels.contains(label)) {
        quint32 parameterId = mParameterLabels.value(label);
        if(mParameters.contains(parameterId)) {
            return mParameters.value(parameterId);
        }
    }
    return param;
}

QHash<quint32, QSharedPointer<QTBParameter>> QTBDataManager::parameters() const
{
    QMutexLocker locker(&mMutex);
    return mParameters;
}

QHash<QString, quint32> QTBDataManager::parameterLabels() const
{
    QMutexLocker locker(&mMutex);
    return mParameterLabels;
}

void QTBDataManager::addSampleUnsafe(quint32 serieIndex, double timestamp, QTBDataValue value)
{
    mDataBuffer->addSample(serieIndex, timestamp, value);
}

void QTBDataManager::addSample(quint32 serieIndex, double timestamp, QTBDataValue value)
{
    QMutexLocker locker(&mMutex);
    mDataBuffer->addSample(serieIndex, timestamp, value);
}

QTBDataSample QTBDataManager::lastSample(quint32 serieIndex)
{
    QMutexLocker locker(&mMutex);
    return mDataBuffer->lastSample(serieIndex);
}

QTBDataSample QTBDataManager::lastSampleUnsafe(quint32 serieIndex)
{
    return mDataBuffer->lastSample(serieIndex);
}

QTBDataSerie QTBDataManager::dataSerie(quint32 serieIndex)
{
    QMutexLocker locker(&mMutex);
    return mDataBuffer->serie(serieIndex);
}

QTBDataSerie QTBDataManager::dataSerieUnsafe(quint32 serieIndex)
{
    return mDataBuffer->serie(serieIndex);
}

void QTBDataManager::updateData()
{
    QMutexLocker locker(&mMutex);
    QMap<QString, DataSourceInterface *>::iterator i;
    for (i = mDataSources.begin(); i != mDataSources.end(); ++i) {
        i.value()->updateDashboardData();
    }

    emit dataUpdated(QDateTime::currentDateTimeUtc());
}

QMap<QString, DataSourceInterface *> QTBDataManager::dataSources() const
{
    return mDataSources;
}

void QTBDataManager::loadDataSources()
{
    QString appPath = qApp->applicationDirPath();
    QString libPath = appPath + QDir::separator() + QString("DataSources");
    QDir pluginsDir(libPath);
    for (const QString& dir: pluginsDir.entryList(QDir::Dirs))
    {
        QDir dataSrcDir(pluginsDir.absoluteFilePath(dir));

        for (const QString& filename: dataSrcDir.entryList(QDir::Files))
        {
            QFileInfo fileinfo(filename);
            if( fileinfo.suffix() != "so" && fileinfo.suffix() != "dll"){
                continue;
            }

            QPluginLoader pluginLoader(dataSrcDir.absoluteFilePath(filename), this);

            QObject *plugin = pluginLoader.instance();
            if (plugin)
            {
                DataSourceInterface *dataSource = qobject_cast<DataSourceInterface *>(plugin);
                if(dataSource) {
                    dataSource->setDataManager(this);
                    dataSource->setCurrentPath(pluginsDir.absoluteFilePath(dir));
                    mDataSources.insert(dir, dataSource);
                    if(dataSource->autoStart())
                        dataSource->startAcquisition();
                }
            }
        }
    }
}


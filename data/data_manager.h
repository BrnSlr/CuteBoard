#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include "data_buffer.h"
#include "data_parameter.h"

#define TEMPO_MS_PARAM_UPDATE 500

class DataSource;
class QTBDataManager : public QObject
{
    Q_OBJECT
public:
    explicit QTBDataManager(QObject *parent = nullptr);
    ~QTBDataManager();

    bool registerParameter(const QSharedPointer<QTBParameter>& param);
    void unregisterParameter(const QSharedPointer<QTBParameter>& param);
    void unregisterParameter(quint32 parameterId);
    void unregisterParameter(const QString& label);

    QSharedPointer<QTBParameter> parameter(quint32 parameterId);
    QSharedPointer<QTBParameter> parameter(const QString& label);

    QHash<quint32, QSharedPointer<QTBParameter> > parameters() const;
    QHash<QString, quint32> parameterLabels() const;

    void addSample(quint32 serieIndex, double timestamp, QTBDataValue value);
    void addSampleUnsafe(quint32 serieIndex, double timestamp, QTBDataValue value);

    QTBDataSample lastSample(quint32 serieIndex);
    QTBDataSample lastSampleUnsafe(quint32 serieIndex);

    QTBDataSerie dataSerie(quint32 serieIndex);
    QTBDataSerie dataSerieUnsafe(quint32 serieIndex);

    void lockData() { mMutex.lock(); }
    void unlockData() { mMutex.unlock(); }

    QMap<QString, DataSource*> dataSources() const;

    QHash<QString, QString> parameterSourceNames() const;

protected:
    void loadDataSources();

signals:
    void parametersUpdated();
    void dataUpdated(QDateTime time);
    void newParameters();
    void updateDashboard();

public slots:
    void updateData();

protected:
    QSharedPointer<QTBDataBuffer> mDataBuffer;
    QHash<quint32, QSharedPointer<QTBParameter>> mParameters;
    QHash<QString, quint32> mParameterLabels;
    QHash<QString, QString> mParameterSourceNames;
    QTimer *mParametersTimer;
    QMap<QString, DataSource *> mDataSources;
    QThread *mThread;
    QTimer *mDataTimer;
    mutable QMutex mMutex;
};

#endif // DATAMANAGER_H

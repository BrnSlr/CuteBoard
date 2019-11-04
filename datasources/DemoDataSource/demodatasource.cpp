#include "demodatasource.h"

DemoDataSource::DemoDataSource():
    DataSourceInterface()
{
    setAutoStart(true);
    mThread = new QThread(this);
    mTimer = new QTimer(nullptr); // _not_ this!
    mTimer->setInterval(10);
    mTimer->setTimerType(Qt::PreciseTimer);
    mTimer->moveToThread(mThread);
    connect(mTimer, SIGNAL(timeout()), SLOT(updateData()), Qt::DirectConnection);
    mTimer->connect(mThread, SIGNAL(started()), SLOT(start()));
    mTimer->connect(mThread, SIGNAL(finished()), SLOT(stop()));

    loadParameters();
}

DemoDataSource::~DemoDataSource()
{
    mThread->quit();
    mThread->wait();
    mTimer->deleteLater();
    mThread->deleteLater();

}

bool DemoDataSource::startAcquisition()
{    
    mThread->start();
    registerParameters();
    return true;
}

bool DemoDataSource::stopAcquisition()
{
    unregisterParameters();
    mThread->quit();
    mThread->wait();
    return true;
}

void DemoDataSource::loadParameters()
{
    for(int i=0;i<2;i++) {

            QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("FLOAT_SINE_%1").arg(mListParam.count())));
            param->setSignalOffset(0.1f);
            param->setSignalAmplitude(float(QRandomGenerator::global()->generateDouble()));
            param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SINE");
            mListParam.append(param);
    }

    for(int i=0;i<2;i++) {

        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("FLOAT_SINE_%1").arg(mListParam.count())));
        param->setSignalOffset(20.0f);
        param->setSignalAmplitude(5.0f*float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SINE/FLOAT");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {

        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("FLOAT_SINE_%1").arg(mListParam.count())));
        param->setSignalOffset(50000.0f);
        param->setSignalAmplitude(3000.0f*float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SINE");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("INT8_TRI_%1").arg(mListParam.count())));
        param->setValueType(QTBDataValue::TYPE_INT8);
        param->setGeneratorType(DemoParameter::gtTriangle);
        param->setSignalOffset(0);
        param->setSignalAmplitude(127.0f *float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("TRI/INT8");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT8_TRI_%1").arg(mListParam.count())));
        param->setValueType(QTBDataValue::TYPE_UINT8);
        param->setGeneratorType(DemoParameter::gtTriangle);
        param->setSignalOffset(127.0f);
        param->setSignalAmplitude(127.0f *float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("TRI/UINT8");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("INT16_SAWT_%1").arg(mListParam.count())));
        param->setValueType(QTBDataValue::TYPE_INT16);
        param->setGeneratorType(DemoParameter::gtSawtooth);
        param->setSignalOffset(0);
        param->setSignalAmplitude(32000.0f *float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SAWT/INT16");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT16_SAWT_%1").arg(mListParam.count())));
        param->setValueType(QTBDataValue::TYPE_UINT16);
        param->setGeneratorType(DemoParameter::gtSawtooth);
        param->setSignalOffset(32000.0f);
        param->setSignalAmplitude(32000.0f *float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SAWT/UINT16");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT16_SQUARE_%1").arg(mListParam.count())));
        param->setValueType(QTBDataValue::TYPE_UINT16);
        param->setGeneratorType(DemoParameter::gtSquare);
        param->setSignalOffset(32000);
        param->setSignalAmplitude(32000 *float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SQUARE/UINT16");
        mListParam.append(param);
    }

    for(int i=0;i<25;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT32_SQUARE_%1").arg(mListParam.count())));
        param->setValueType(QTBDataValue::TYPE_UINT32);
        param->setGeneratorType(DemoParameter::gtSquare);
        param->setSignalOffset(123456878);
        param->setSignalAmplitude(2315687 *float(QRandomGenerator::global()->generateDouble()));
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("SQUARE/UINT32");
        mListParam.append(param);
    }

}

void DemoDataSource::registerParameters()
{
    QList<QSharedPointer<DemoParameter>>::iterator i;
    for (i = mListParam.begin(); i != mListParam.end(); ++i) {
        registerParameter((*i));
    }
}

void DemoDataSource::unregisterParameters()
{
    QList<QSharedPointer<DemoParameter>>::iterator i;
    for (i = mListParam.begin(); i != mListParam.end(); ++i)
        unregisterParameter((*i)->parameterId());
}

void DemoDataSource::updateData()
{
    double timestamp = QDateTime::currentDateTimeUtc().time().msecsSinceStartOfDay() / 1000.0;
    QList<QSharedPointer<DemoParameter>>::iterator i;
    for (i = mListParam.begin(); i != mListParam.end(); ++i) {
        updateSample((*i)->parameterId(),
                     timestamp,
                     (*i)->updateParameter(timestamp));
    }
}

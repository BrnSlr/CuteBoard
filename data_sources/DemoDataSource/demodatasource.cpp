#include "demodatasource.h"
#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QDateTime>

DemoDataSource::DemoDataSource():
    LiveDataSourceInterface("DemoLive")
{
    //    setIsTimeReference(true);
    mPeriodMillisec = 1;
    loadParameters();

    mThread = new QThread(this);
    mTimer = new QTimer(nullptr); // _not_ this!
    mTimer->setInterval(mPeriodMillisec);
    mTimer->setTimerType(Qt::PreciseTimer);
    mTimer->moveToThread(mThread);
    connect(mTimer, SIGNAL(timeout()), SLOT(updateData()), Qt::DirectConnection);
    mTimer->connect(mThread, SIGNAL(started()), SLOT(start()));
    mTimer->connect(mThread, SIGNAL(finished()), SLOT(stop()));
}

DemoDataSource::~DemoDataSource()
{
    mThread->quit();
    mThread->wait();
    mTimer->deleteLater();
    mThread->deleteLater();

}

bool DemoDataSource::startLiveAcquisition()
{    
    registerParameters();
    mThread->start();
    return true;
}

bool DemoDataSource::stopLiveAcquisition()
{
    mThread->quit();
    mThread->wait();
    unregisterParameters();

    return true;
}

void DemoDataSource::loadParameters()
{

    for(int i=0;i<5;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UnsignedInteger8_Triangle_%1").arg(i)));
        param->setValueType(DataValue::TYPE_UINT8);
        param->setGeneratorType(DemoParameter::gtTriangle);
        param->setSignalOffset(i);
        param->setSignalAmplitude(i *QRandomGenerator::global()->generateDouble());
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("Unsigned/Triangle");
        param->setRefreshPeriod(mPeriodMillisec);
        param->setSamplesPerPeriod(1);
        mListDemoParam.append(param);
    }

//    for(int i=0;i<10;i++) {
//        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT8_TRI_%1").arg(i)));
//        param->setValueType(DataValue::TYPE_UINT8);
//        param->setGeneratorType(DemoParameter::gtTriangle);
//        param->setSignalOffset(20);
//        param->setSignalAmplitude(25 *QRandomGenerator::global()->generateDouble());
//        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
//        param->setSourceName("TRI/UINT8");
//        param->setRefreshPeriod(mPeriodMillisec);
//        param->setSamplesPerPeriod(1);
//        mListDemoParam.append(param);
//    }

//    for(int i=0;i<10;i++) {
//        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("INT16_SAWT_%1").arg(i)));
//        param->setValueType(DataValue::TYPE_INT16);
//        param->setGeneratorType(DemoParameter::gtSawtooth);
//        param->setSignalOffset(20);
//        param->setSignalAmplitude(25*QRandomGenerator::global()->generateDouble());
//        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
//        param->setSourceName("SAWT/INT16");
//        param->setRefreshPeriod(mPeriodMillisec);
//        param->setSamplesPerPeriod(1);
//        mListDemoParam.append(param);
//    }

//    for(int i=0;i<10;i++) {
//        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT16_SAWT_%1").arg(i)));
//        param->setValueType(DataValue::TYPE_UINT16);
//        param->setGeneratorType(DemoParameter::gtSawtooth);
//        param->setSignalOffset(20);
//        param->setSignalAmplitude(25);
//        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
//        param->setSourceName("SAWT/UINT16");
//        param->setRefreshPeriod(mPeriodMillisec);
//        param->setSamplesPerPeriod(1);
//        mListDemoParam.append(param);
//    }

    for(int i=0;i<5;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UnsignedInteger16_Square01_%1").arg(i)));
        param->setValueType(DataValue::TYPE_UINT16);
        param->setGeneratorType(DemoParameter::gtSquare);
        param->setSignalOffset(0);
        param->setSignalAmplitude(1);
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("Unsigned/Square");
        param->setRefreshPeriod(mPeriodMillisec);
        param->setSamplesPerPeriod(1);
        mListDemoParam.append(param);
    }

//    for(int i=0;i<10;i++) {
//        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("UINT32_SQUARE_%1").arg(i)));
//        param->setValueType(DataValue::TYPE_UINT32);
//        param->setGeneratorType(DemoParameter::gtSquare);
//        param->setSignalOffset(20);
//        param->setSignalAmplitude(25 *QRandomGenerator::global()->generateDouble());
//        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
//        param->setSourceName("SQUARE/UINT32");
//        param->setRefreshPeriod(mPeriodMillisec);
//        param->setSamplesPerPeriod(1);
//        mListDemoParam.append(param);
//    }

//    for(int i=0;i<10;i++) {
//        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("DOUBLE_SINE_%1").arg(i)));
//        param->setValueType(DataValue::TYPE_DOUBLE);
//        param->setSignalOffset(20.0);
//        param->setSignalAmplitude(25.0*QRandomGenerator::global()->generateDouble());
//        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
//        param->setSourceName("SINE/DOUBLE");
//        param->setRefreshPeriod(mPeriodMillisec);
//        param->setSamplesPerPeriod(1);
//        mListDemoParam.append(param);
//    }

//    for(int i=0;i<10;i++) {
//        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("FLOAT_SINE_%1").arg(i)));
//        param->setValueType(DataValue::TYPE_FLOAT);
//        param->setSignalOffset(20.0);
//        param->setSignalAmplitude(25.0*QRandomGenerator::global()->generateDouble());
//        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
//        param->setSourceName("SINE/FLOAT");
//        param->setRefreshPeriod(mPeriodMillisec);
//        param->setSamplesPerPeriod(1);
//        mListDemoParam.append(param);
//    }

    for(int i=0;i<40;i++) {
        QSharedPointer<DemoParameter> param = QSharedPointer<DemoParameter>(new DemoParameter(QString("Double_Custom_%1").arg(i)));
        param->setValueType(DataValue::TYPE_DOUBLE);
        param->setGeneratorType(DemoParameter::gtCustom);
        param->setSignalOffset(20.0);
        param->setSignalAmplitude(25.0*QRandomGenerator::global()->generateDouble());
        param->setSignalPhase(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
        param->setSourceName("Double/Custom");
        param->setRefreshPeriod(mPeriodMillisec);
        param->setSamplesPerPeriod(1);
        mListDemoParam.append(param);
    }
}

void DemoDataSource::registerParameters()
{
    QList<QSharedPointer<DemoParameter>>::iterator i;
    for (auto i : mListDemoParam) {
        registerParameter(i);
    }

}

void DemoDataSource::unregisterParameters()
{
    QList<QSharedPointer<DemoParameter>>::iterator i;
    for (i = mListDemoParam.begin(); i != mListDemoParam.end(); ++i)
        unregisterParameter((*i)->parameterId());
}

void DemoDataSource::updateData()
{
    auto t = QDateTime::currentDateTimeUtc();
    double timestamp = t.toMSecsSinceEpoch() / 1000.0;
    setReferenceTime(t);

    QList<QSharedPointer<DemoParameter>>::iterator i;
    for (i = mListDemoParam.begin(); i != mListDemoParam.end(); ++i) {
        (*i)->updateParameterVec(timestamp);
    }
}

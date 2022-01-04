#include "alarms.h"
#include "ui/board/management/utils/string_util.h"
#include "log4qt/logger.h"
#include "log4qt/logmanager.h"

LOG4QT_DECLARE_STATIC_LOGGER(logger, Alarms)

Alarms::Alarms()
{
    mModified = false;
}

void Alarms::saveToFile(const QString &saveDirectory)
{    
    if(mSaveFilePath.isEmpty()) {
        mSaveFilePath = saveDirectory + QDir::separator() + mName + QString(".alrm");
    }

    logger()->info() << QString("Saving AlarmConfiguration [%1] to file [%2]").arg(mName).arg(mSaveFilePath);

    QSettings settings(mSaveFilePath, QSettings::IniFormat);
    settings.clear();
    settings.sync();

    save(&settings);
}

void Alarms::loadFromFile(const QString &settingsFile)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    load(&settings);
    mSaveFilePath = settingsFile;
}

void Alarms::save(QSettings *settings)
{
    settings->beginWriteArray("Alarms");
    {
        for(int i=0; i< mActive.count();i++) {
            settings->setArrayIndex(i);
            settings->setValue("Active", mActive.at(i));
            settings->setValue("WatchedParameter", mWatchedParameters.at(i));
            settings->setValue("Message", mMessages.at(i));
            settings->setValue("OutputParameter", mOutputParameters.at(i));
            settings->setValue("Function", mFunctions.at(i));
            settings->setValue("FunctionArg", mFunctionArgs.at(i));
            settings->setValue("Color", mColors.at(i).name(QColor::HexArgb));
        }
    }
    settings->endArray();
}

void Alarms::load(QSettings *settings)
{
    //    mName= settings->value("Name").toString();
    auto fileInfo = QFileInfo(settings->fileName());
    mName = fileInfo.completeBaseName();
    clearAlarms();
    int alarmsCount = settings->beginReadArray("Alarms");
    {
        for(int i=0;i<alarmsCount;i++) {
            settings->setArrayIndex(i);

            addAlarm(settings->value("Active").toBool(),
                     settings->value("WatchedParameter").toString(),
                     settings->value("Message").toString(),
                     settings->value("OutputParameter").toString(),
                     Alarms::AlarmFunction(settings->value("Function").toInt()),
                     settings->value("FunctionArg").toDouble(),
                     settings->value("Color").toString());
        }

    }
    settings->endArray();
}

QString Alarms::name() const
{
    return mName;
}

void Alarms::setName(const QString &name)
{
    mName = name;
}

void Alarms::clearAlarms()
{
    mActive.clear();
    mWatchedParameters.clear();
    mMessages.clear();
    mOutputParameters.clear();
    mFunctions.clear();
    mFunctionArgs.clear();
    mColors.clear();
}

void Alarms::addAlarm(bool active, QString watchedParam, QString message, QString mOutputParam, Alarms::AlarmFunction function, double arg, QColor color)
{
    mActive.append(active);
    mWatchedParameters.append(watchedParam);
    mMessages.append(message);
    mOutputParameters.append(mOutputParam);
    mFunctions.append(function);
    mFunctionArgs.append(arg);
    mColors.append(color);
}

QList<bool> Alarms::active() const
{
    return mActive;
}

QList<QString> Alarms::watchedParameters() const
{
    return mWatchedParameters;
}

QList<QString> Alarms::messages() const
{
    return mMessages;
}

QList<QString> Alarms::outputParameters() const
{
    return mOutputParameters;
}

QList<Alarms::AlarmFunction> Alarms::functions() const
{
    return mFunctions;
}

QList<double> Alarms::functionArgs() const
{
    return mFunctionArgs;
}

QList<QColor> Alarms::colors() const
{
    return mColors;
}

bool Alarms::modified() const
{
    return mModified;
}

void Alarms::setModified(bool modified)
{
    mModified = modified;
}

QString Alarms::saveFilePath() const
{
    return mSaveFilePath;
}

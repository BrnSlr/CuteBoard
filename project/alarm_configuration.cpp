#include "alarm_configuration.h"
#include "project/string_util.h"

QTBAlarmConfiguration::QTBAlarmConfiguration()
{
    mModified = false;
}

void QTBAlarmConfiguration::saveToFile(const QString &saveDirectory)
{
    QString name = mName;
    name = removeSpecialChars(name);

    QString settingsPath = saveDirectory + QDir::separator() + name + QString(".alrm");

    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.clear();
    settings.sync();

    save(&settings);
}

void QTBAlarmConfiguration::loadFromFile(const QString &settingsFile)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    load(&settings);
}

void QTBAlarmConfiguration::save(QSettings *settings)
{
    settings->setValue("Name", mName);
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
            settings->setValue("Color", mColors.at(i).name());
        }
    }
    settings->endArray();
}

void QTBAlarmConfiguration::load(QSettings *settings)
{
    mName= settings->value("Name").toString();
    clearAlarms();
    int alarmsCount = settings->beginReadArray("Alarms");
    {

        for(int i=0;i<alarmsCount;i++) {
            settings->setArrayIndex(i);

            addAlarm(settings->value("Active").toBool(),
                     settings->value("WatchedParameter").toString(),
                     settings->value("Message").toString(),
                     settings->value("OutputParameter").toString(),
                     QTBAlarmConfiguration::AlarmFunction(settings->value("Function").toInt()),
                     settings->value("FunctionArg").toDouble(),
                     settings->value("Color").toString());
        }

    }
    settings->endArray();
}

QString QTBAlarmConfiguration::name() const
{
    return mName;
}

void QTBAlarmConfiguration::setName(const QString &name)
{
    mName = name;
}

void QTBAlarmConfiguration::clearAlarms()
{
    mActive.clear();
    mWatchedParameters.clear();
    mMessages.clear();
    mOutputParameters.clear();
    mFunctions.clear();
    mFunctionArgs.clear();
    mColors.clear();
}

void QTBAlarmConfiguration::addAlarm(bool active, QString watchedParam, QString message, QString mOutputParam, QTBAlarmConfiguration::AlarmFunction function, double arg, QColor color)
{
    mActive.append(active);
    mWatchedParameters.append(watchedParam);
    mMessages.append(message);
    mOutputParameters.append(mOutputParam);
    mFunctions.append(function);
    mFunctionArgs.append(arg);
    mColors.append(color);
}

QList<bool> QTBAlarmConfiguration::active() const
{
    return mActive;
}

QList<QString> QTBAlarmConfiguration::watchedParameters() const
{
    return mWatchedParameters;
}

QList<QString> QTBAlarmConfiguration::messages() const
{
    return mMessages;
}

QList<QString> QTBAlarmConfiguration::outputParameters() const
{
    return mOutputParameters;
}

QList<QTBAlarmConfiguration::AlarmFunction> QTBAlarmConfiguration::functions() const
{
    return mFunctions;
}

QList<double> QTBAlarmConfiguration::functionArgs() const
{
    return mFunctionArgs;
}

QList<QColor> QTBAlarmConfiguration::colors() const
{
    return mColors;
}

bool QTBAlarmConfiguration::modified() const
{
    return mModified;
}

void QTBAlarmConfiguration::setModified(bool modified)
{
    mModified = modified;
}

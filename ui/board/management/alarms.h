#ifndef ALARM_CONFIGURATION_H
#define ALARM_CONFIGURATION_H

#include <QString>
#include <QSettings>
#include "3rdparty/qcustomplot/qcustomplot.h"
#include "ui/ui_global.h"

class BOARD_UI_EXPORT Alarms : public QSharedData
{
public:
    enum AlarmFunction {
        afGreaterThan,
        afLowerThan,
        afBitSet,
        afParamDead
    };

    Alarms();

    void saveToFile(const QString& saveDirectory);
    void loadFromFile(const QString& settingsFile);

    void save(QSettings *settings);
    void load(QSettings *settings);

    QString name() const;
    void setName(const QString &name);

    int alarmsCount() {return mActive.count();}
    void clearAlarms();

    void addAlarm(bool active,
                  QString watchedParam,
                  QString message,
                  QString mOutputParam,
                  AlarmFunction function,
                  double arg,
                  QColor color);

    QList<bool> active() const;
    QList<QString> watchedParameters() const;
    QList<QString> messages() const;
    QList<QString> outputParameters() const;
    QList<AlarmFunction> functions() const;
    QList<double> functionArgs() const;
    QList<QColor> colors() const;

    bool modified() const;
    void setModified(bool modified);

    QString saveFilePath() const;

protected:
    QString mSaveFilePath;
    QString mName;
    QList<bool> mActive;
    QList<QString> mWatchedParameters;
    QList<QString> mMessages;
    QList<QString> mOutputParameters;
    QList<AlarmFunction> mFunctions;
    QList<double> mFunctionArgs;
    QList<QColor> mColors;
    bool mModified;
};

#endif // ALARM_CONFIGURATION_H

#ifndef TIME_BUTTON_H
#define TIME_BUTTON_H

#include <QToolButton>
#include <QDateTime>
#include <QStyle>
#include <QDebug>
#include "log4qt/logger.h"

class TimeButton : public QToolButton
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    public:
        enum DateTimeFormat {
                 dtfTimeSec,
                 dtfTimeMSec,
                 dtfDateTimeSec,
                 dtfDateTimeMSec };

    TimeButton(QWidget *parent = nullptr);
    ~TimeButton() Q_DECL_OVERRIDE;

    void setDateTime(const QDateTime &dateTime);
    void setBlinking(bool blink);

public slots:
    void rotateFormat();

protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    QDateTime mDateTime;
    int mTimerId;
    DateTimeFormat mDateTimeFormat;
    QString mDateTimeFormatString;
    bool mFlashing;

    QPalette mPalette;
    QPalette mBlinkingPalette;
};

#endif // TIME_BUTTON_H

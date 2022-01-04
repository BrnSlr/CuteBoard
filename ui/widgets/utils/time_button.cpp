#include "time_button.h"

TimeButton::TimeButton(QWidget *parent) :
    QToolButton(parent),
    mTimerId(0),
    mDateTimeFormat(dtfTimeSec),
    mDateTimeFormatString(QString("hh:mm:ss")),
    mFlashing(false)
{
    setObjectName("TimeButton");
    connect(this, &QToolButton::clicked,
            this, &TimeButton::rotateFormat);

    mPalette = palette();
    mBlinkingPalette = mPalette;

    mBlinkingPalette.setColor(QPalette::ButtonText, mPalette.highlight().color());
}

TimeButton::~TimeButton()
{

}

void TimeButton::rotateFormat()
{
    mDateTimeFormat = static_cast<DateTimeFormat>((mDateTimeFormat + 1) % (DateTimeFormat::dtfDateTimeMSec + 1));

    switch(mDateTimeFormat)
    {
        case dtfTimeSec:
            mDateTimeFormatString = QString("hh:mm:ss");
            break;
        case dtfTimeMSec:
            mDateTimeFormatString = QString("hh:mm:ss.zzz");
            break;
        case dtfDateTimeSec:
            mDateTimeFormatString = QString("dd.MM.yy hh:mm:ss");
            break;
        case dtfDateTimeMSec:
            mDateTimeFormatString = QString("dd.MM.yy hh:mm:ss.zzz");
            break;
    }
    setText(mDateTime.toString(mDateTimeFormatString));
}

void TimeButton::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    mFlashing = !mFlashing;
    if(mFlashing)
        setPalette(mBlinkingPalette);
    else
        setPalette(mPalette);
}

void TimeButton::setDateTime(const QDateTime &dateTime)
{
    mDateTime = dateTime;
    setText(mDateTime.toString(mDateTimeFormatString));
}

void TimeButton::setBlinking(bool blink)
{
    mFlashing = blink;
    if(mFlashing)
        setPalette(mBlinkingPalette);
    else
        setPalette(mPalette);
    if(blink && mTimerId == 0) {
        mTimerId = startTimer(500);
    } else {
        killTimer(mTimerId);
        mTimerId = 0;
    }
}

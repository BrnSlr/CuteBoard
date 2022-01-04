/*
    Copyright (C) 2014 by Elvis Angelaccio <elvis.angelaccio@kde.org>

    This file is part of Kronometer.

    Kronometer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Kronometer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Kronometer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMEDISPLAY_H
#define TIMEDISPLAY_H

#include "ui_timedisplay.h"
#include "timeformat.h"

#include <QTime>
#include <QWidget>

class DigitDisplay;
class QGroupBox;

/**
 * @brief A custom widget displaying a QTime
 * This custom widget implements a "digital" display for a time, formatted according to a certain format.
 * This widget can be connected to a generic "time source" producing the time to be displayed.
 */
class TimeDisplay : public QWidget, public Ui::TimeDisplay
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:

    explicit TimeDisplay(QWidget *parent = nullptr);

    /**
     * Set the internal time format of the display
     * @param format The format to be used as time format.
     */
    void setTimeFormat(const TimeFormat& format);

    void setDisplayFont(const QFont& font);
    /**
     * Set a custom font for hours
     * @param font The custom font to set.
     */
    void setHoursFont(const QFont& font);

    /**
     * Set a custom font for minutes
     * @param font The custom font to set.
     */
    void setMinutesFont(const QFont& font);

    /**
     * Set a custom font for seconds
     * @param font The custom font to set.
     */
    void setSecondsFont(const QFont& font);

    /**
     * Set a custom font for second fractions
     * @param font The custom font to set.
     */
    void setFractionsFont(const QFont& font);

       /**
     * Get the current time formatted with the current format.
     * @return Current time formatted as string.
     */
    QString currentTime();

public slots:

    /**
     * Set the time to be displayed.
     * @param time The time to be displayed.
     */
    void setTime(int time);

    /**
     * Reset the display to the default time format. The overridden format (if any) is lost.
     */
    void reset();

private:

    static constexpr int MSECS_PER_HOUR = 3600000;
    static constexpr int MSECS_PER_MIN = 60000;
    static constexpr int MSECS_PER_SEC = 1000;
    static constexpr int SECS_PER_MIN = 60;

    QTime m_displayTime = {0, 0};     /** Current display time */
    TimeFormat m_currentFormat;       /** Current display time format. */
    TimeFormat m_defaultFormat;       /** Default time format, to be restored on reset. */

    /**
     * Refresh the labels text implementing the display timer
     */
    void updateTimer();

    /**
     * Refresh the minimum width of the frames, based on current font sizes
     */
    void updateWidth();

    /**
     * Helper function, called when setting and overriding the time format.
     */
    void updateTimeFormat();

    Q_DISABLE_COPY(TimeDisplay)
};

#endif

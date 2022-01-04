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

#include "timedisplay.h"
#include "digitdisplay.h"

#include <QBoxLayout>
#include <QGroupBox>

TimeDisplay::TimeDisplay(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    m_hourDisplay->setDigits(DigitDisplay::Digits::Two);
    m_minDisplay->setDigits(DigitDisplay::Digits::Two);
    m_secDisplay->setDigits(DigitDisplay::Digits::Two);

    m_hourDisplay->showDigits(m_currentFormat.formatHours(m_displayTime));
    m_minDisplay->showDigits(m_currentFormat.formatMinutes(m_displayTime));
    m_secDisplay->showDigits(m_currentFormat.formatSeconds(m_displayTime));
    m_fracDisplay->showDigits(m_currentFormat.formatFractions(m_displayTime));
}

void TimeDisplay::setTimeFormat(const TimeFormat& format)
{
    m_defaultFormat = format;
    m_currentFormat = format;
    updateTimeFormat();
}

void TimeDisplay::setDisplayFont(const QFont &font)
{
    setHoursFont(font);
    setMinutesFont(font);
    setSecondsFont(font);
    setFractionsFont(font);
}

void TimeDisplay::setHoursFont(const QFont& font)
{
    m_hourDisplay->setFont(font);
    updateWidth();
}

void TimeDisplay::setMinutesFont(const QFont& font)
{
    m_minDisplay->setFont(font);
    updateWidth();
}

void TimeDisplay::setSecondsFont(const QFont& font)
{
    m_secDisplay->setFont(font);
    updateWidth();
}

void TimeDisplay::setFractionsFont(const QFont& font)
{
    m_fracDisplay->setFont(font);
    updateWidth();
}

QString TimeDisplay::currentTime()
{
    m_currentFormat.showDividers(true);
    const auto currentTime = m_currentFormat.format(m_displayTime);
    m_currentFormat.showDividers(false);

    return currentTime;
}

void TimeDisplay::setTime(int time)
{
    m_displayTime.setHMS(time / MSECS_PER_HOUR,
                        (time % MSECS_PER_HOUR) / MSECS_PER_MIN,
                        (time / MSECS_PER_SEC) % 60,
                        time % MSECS_PER_SEC);

    updateTimer();
}

void TimeDisplay::reset()
{
    if (m_currentFormat == m_defaultFormat)
        return;

    setTimeFormat(m_defaultFormat);
}

void TimeDisplay::updateTimer()
{
    if (m_currentFormat.hasHours()) {
        m_hourDisplay->showDigits(m_currentFormat.formatHours(m_displayTime));
    }
    else if (m_displayTime.hour() >= 1) {
        m_currentFormat.overrideHours();
        updateTimeFormat();
    }

    if (m_currentFormat.hasMinutes()) {
        m_minDisplay->showDigits(m_currentFormat.formatMinutes(m_displayTime));
    }
    else if (m_displayTime.minute() >= 1) {
        m_currentFormat.overrideMinutes();
        updateTimeFormat();
    }

    m_secDisplay->showDigits(m_currentFormat.formatSeconds(m_displayTime));

    if (m_currentFormat.hasFractions()) {
        m_fracDisplay->showDigits(m_currentFormat.formatFractions(m_displayTime));
    }
}

void TimeDisplay::updateWidth()
{
    auto width = qMax(qMax(m_hourDisplay->minimumSizeHint().width(),
                          m_minDisplay->minimumSizeHint().width()),
                     qMax(m_secDisplay->minimumSizeHint().width(),
                          m_fracDisplay->minimumSizeHint().width()));

    width = width + (width * 20 / 100); // 20% as padding, i.e. 10% as right padding and 10% as left padding

    m_hourDisplay->setMinimumWidth(width);
    m_minDisplay->setMinimumWidth(width);
    m_secDisplay->setMinimumWidth(width);
    m_fracDisplay->setMinimumWidth(width);
}

void TimeDisplay::updateTimeFormat()
{
    m_hourDisplay->setVisible(m_currentFormat.hasHours());
    m_minDisplay->setVisible(m_currentFormat.hasMinutes());
    m_fracDisplay->setVisible(m_currentFormat.hasFractions());

    if (m_currentFormat.hasFractions()) {
        switch (m_currentFormat.secondFractions()) {
        case TimeFormat::UpToTenths:
            m_fracDisplay->setDigits(DigitDisplay::Digits::One);
            break;
        case TimeFormat::UpToHundredths:
            m_fracDisplay->setDigits(DigitDisplay::Digits::Two);
            break;
        case TimeFormat::UpToMilliseconds:
            m_fracDisplay->setDigits(DigitDisplay::Digits::Three);
            break;
        default:
            break;
        }
    }

    updateTimer();
}

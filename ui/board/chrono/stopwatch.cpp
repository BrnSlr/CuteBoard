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

#include "stopwatch.h"

#include <QCoreApplication>
#include <QTimerEvent>
#include <QDebug>

Stopwatch::Stopwatch(QObject *parent) : QObject(parent) {}

void Stopwatch::setGranularity(Granularity g)
{
    m_granularity = g;

    // whenever granularity is changed, also apply that granularity to the timer event
    if (m_timerId != INACTIVE_TIMER_ID) {
        killTimer(m_timerId);
        m_timerId = startTimer(granularity());
    }
}

bool Stopwatch::isRunning() const
{
    return m_state == State::Running;
}

bool Stopwatch::isPaused() const
{
    return m_state == State::Paused;
}

bool Stopwatch::isInactive() const
{
    return m_state == State::Inactive;
}

int Stopwatch::raw() const
{
    return m_accumulator;
}

bool Stopwatch::initialize(int rawData)
{
    if (! isInactive() || rawData <= 0) {
        return false;
    }

    m_accumulator = rawData;
    m_state = State::Paused;
    emit time(m_accumulator);  // it signals that has been deserialized and can be resumed

    return true;
}

void Stopwatch::start()
{
    if (isInactive()) {
        m_accumulator = 0;
        m_elapsedTimer.start();

        if (m_timerId == INACTIVE_TIMER_ID) {
            m_timerId = startTimer(granularity());
        }
    }
    else if (isPaused()) {
        m_elapsedTimer.restart();
        m_timerId = startTimer(granularity());
    }

    m_state = State::Running;
    emit running();
}

void Stopwatch::pause()
{
    if (m_elapsedTimer.isValid()) {
        m_accumulator += m_elapsedTimer.elapsed();
    }

    m_elapsedTimer.invalidate();
    m_state = State::Paused;
    emit paused();
}

void Stopwatch::reset()
{
    m_elapsedTimer.invalidate();          // if state is running, it will emit a zero time at next timerEvent() call
    QCoreApplication::processEvents();
    emit time(0);
    emit laptime(0);
    m_state = State::Inactive;
    emit inactive();
}

void Stopwatch::storeLap()
{
    auto lapTime = m_accumulator;

    if (m_elapsedTimer.isValid()) {
        lapTime += m_elapsedTimer.elapsed();
    }

    m_currentLap = lapTime;

    const auto zero = QTime {0, 0};
    emit lap(zero.addMSecs(lapTime));
}

void Stopwatch::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != m_timerId) {      // forward undesired events
        QObject::timerEvent(event);
        return;
    }

    if (m_elapsedTimer.isValid()) {
        emit time(m_accumulator + m_elapsedTimer.elapsed());
        emit laptime(m_accumulator + m_elapsedTimer.elapsed() - m_currentLap);
    }
    else {
        killTimer(m_timerId);
        m_timerId = INACTIVE_TIMER_ID;
    }
}

int Stopwatch::granularity() const
{
    return static_cast<int>(m_granularity);
}

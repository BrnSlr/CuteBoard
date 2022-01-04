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

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QElapsedTimer>
#include <QObject>
#include <QTime>
#include "log4qt/logger.h"

class QTimerEvent;

/**
 * @brief A Stopwatch class.
 * Stopwatch is a simple QObject implementing a real stopwatch.
 * The class provides public slots to start/pause/reset its internal timer.
 * A slot for laps recording exists too, but the computing of lap times is not a task of this class:
 * Stopwatch simply emits a signal, that the receiver can use to compute lap times.
 */
class Stopwatch : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:

    enum class Granularity : int
    {
        Milliseconds = 1, /**< Stopwatch refreshed every msec. */
        Hundredths = 10,  /**< Stopwatch refreshed every 10 msec. */
        Tenths = 100,     /**< Stopwatch refreshed every 100 msec. */
        Seconds = 1000   /**< Stopwatch refreshed every sec. */
    };

    explicit Stopwatch(QObject *parent = nullptr);

    /**
     * Set the stopwatch refresh granularity
     * @param g The granularity to be set.
     */
    void setGranularity(Granularity g);

    /**
     * Check if the stopwatch is running
     * @return true if running, false otherwise
     */
    bool isRunning() const;

    /**
     * Check if the stopwatch is paused
     * @return true if paused, false otherwise
     */
    bool isPaused() const;

    /**
     * Check if the stopwatch is inactive
     * @return true if inactive, false otherwise
     */
    bool isInactive() const;

    /**
     * Read-only access to the stopwatch underlying data
     * @return The stopwatch raw counter
     */
    int raw() const;

    /**
     * (Re)-initialize (deserialize) the stopwatch from the given raw data counter.
     * Only an inactive stopwatch is meant to be (re)-initialized (deserialized).
     * @param rawData The raw milliseconds counter for the stopwatch
     * @return true if the operation succeeds (i.e. the stopwatch was inactive), false otherwise
     */
    bool initialize(int rawData);

public slots:

    /**
     * Start the stopwatch, if inactive or paused.
     */
    void start();

    /**
     * Pause the stopwatch, if running.
     */
    void pause();

    /**
     * Reset the stopwatch to the inactive state.
     */
    void reset();

    /**
     * Tells the stopwatch to compute a new lap time.
     */
    void storeLap();

signals:

    /**
     * The stopwatch has been started.
     */
    void running();

    /**
     * The stopwatch has been paused.
     */
    void paused();

    /**
     * The stopwatch has been reset.
     */
    void inactive();

    /**
     * Emits a signal with the last lap *absolute* time.
     * This class does not compute *relatives* lap times.
     * You can compute them simply by the difference between consecutives absolute times.
     * @param lapTime The absolute time of the last lap.
     */
    void lap(const QTime& lapTime);

    /**
     * Emits a signal with the current stopwatch time.
     * @param t Current stopwatch time.
     */
    void time(int t);

    /**
     * Emits a signal with the current stopwatch time.
     * @param t Current stopwatch time.
     */
    void laptime(int t);

protected:

    void timerEvent(QTimerEvent *event) override;

private:

    int granularity() const;

    enum class State
    {
        Inactive, /**< Inactive stopwatch. */
        Running,  /**< Running stopwatch. */
        Paused    /**< Paused stopwatch. */
    };

    static constexpr int INACTIVE_TIMER_ID = -1;    /** Used for timerId initialization */

    int m_timerId = INACTIVE_TIMER_ID;                      /** ID for the QObject timer */
    int m_accumulator = 0;                                  /** milliseconds internal counter */
    int m_currentLap = 0;                                  /** milliseconds internal counter */
    State m_state = State::Inactive;                        /** Stopwatch current state */
    Granularity m_granularity = Granularity::Hundredths;    /** Stopwatch current granularity */

    QElapsedTimer m_elapsedTimer;               /** Stopwatch core class*/

    Q_DISABLE_COPY(Stopwatch)
};



#endif

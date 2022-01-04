/*
    Copyright (C) 2015 by Elvis Angelaccio <elvis.angelaccio@kde.org>

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

#ifndef SESSION_H
#define SESSION_H

#include "lap.h"

#include <QMetaType>
#include <QVector>

/**
 * @brief A session represents a single Kronometer instance.
 * A session is made by a stopwatch time, a date timestamp and by zero or more laps.
 * An optional name and an optional note are also provided.
 */
class Session
{

public:

    explicit Session(int time = 0, const QDateTime& date = QDateTime::currentDateTime());

    /**
     * Set the session's name.
     * @param name The string to be set as name.
     */
    void setName(const QString& name);

    /**
     * Set the session's annotation.
     * @param note The note to be set.
     */
    void setNote(const QString& note);

    /**
     * Set the session's stopwatch (raw) time.
     * @param time The stopwatch raw time.
     */
    void setTime(int time);

    /**
     * Set the session's date (timestamp).
     * @param date The date to be set as timestamp.
     */
    void setDate(const QDateTime& date);

    /**
     * Set whether the session data is outdated.
     * @param isOutdated Whether the session is outdated.
     */
    void setIsOutdated(bool isOutdated);

    /**
     * @return The session name.
     */
    QString name() const;

    /**
     * @return The session note.
     */
    QString note() const;

    /**
     * @return The session stopwatch raw time.
     */
    int time() const;

    /**
     * @return The session date.
     */
    QDateTime date() const;

    /**
     * Whether the session data is outdated.
     * @return True if the session is outdated, false otherwise.
     */
    bool isOutdated() const;

    /**
     * @return The session laps.
     */
    QVector<Lap> laps() const;

    /**
     * Whether the session is empty.
     * @return True if the session is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * Add a lap to the session.
     * @param lap The lap the be added.
     */
    void addLap(const Lap& lap);

    /**
     * Reset the stopwatch time and remove all the laps from the session.
     */
    void clear();

    /**
     * Serialize the session on the given JSON object.
     * @param json A JSON object.
     */
    void write(QJsonObject& json) const;

    /**
     * Deserialize a session from the given JSON object.
     * @param json A JSON object.
     * @return A deserialized session.
     */
    static Session fromJson(const QJsonObject& json);

    bool operator==(const Session& right) const;
    bool operator!=(const Session& right) const;

private:

    QString m_name;       /** Session name. */
    QString m_note;       /** Custom session annotation. */
    int  m_time;          /** Session stopwatch time. */
    QDateTime m_date;     /** Session date (timestamp of the last update). */
    QVector<Lap> m_laps;    /** Session laps. */
    bool m_isOutdated = false;    /** Whether the session data is outdated. */
};

Q_DECLARE_METATYPE(Session)

#endif

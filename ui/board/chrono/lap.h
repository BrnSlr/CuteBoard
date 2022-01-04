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

#ifndef LAP_H
#define LAP_H

#include <QMetaType>
#include <QString>
#include <QTime>

class QJsonObject;

/**
 * @brief A Lap is a specific time instant.
 * This class is a wrapper for a QTime object and some strings, which are useful to describe it.
 */
class Lap
{

public:

    explicit Lap(const QTime& lap = {0, 0});

    /**
     * The specific lap's time
     * @return The underlying lap's time object
     */
    QTime time() const;

    /**
     * Compute the difference with the given Lap.
     * @param lap A Lap object.
     * @return QTime difference if the given Lap is "greater", otherwise a zero QTime.
     */
    QTime timeTo(const Lap& lap) const;

    /**
     * Set the lap's relative time
     * @param rel The string to be set as relative time
     */
    void setRelativeTime(const QString& rel);

    /**
     * The relative lap time
     * @return String representation of the relative lap time
     */
    QString relativeTime() const;

    /**
     * Set the lap's absolute time
     * @param abs The string to be set as absolute lap time
     */
    void setAbsoluteTime(const QString& abs);

    /**
     * The absolute lap time
     * @return String representation of the absolute lap time
     */
    QString absoluteTime() const;

    /**
     * Set the lap's annotation
     * @param note The note to be set
     */
    void setNote(const QString& note);

    /**
     * The lap's annotation
     * @return The lap's annotation
     */
    QString note() const;

    /**
     * The underlying lap's raw data
     * @return Lap's raw data counter
     */
    int raw() const;

    /**
     * Serialize the lap on the given JSON object.
     * @param json A JSON object.
     */
    void write(QJsonObject& json) const;

    /**
     * Deserialize a lap from the given JSON object.
     * @param json A JSON object.
     * @return A deserialized lap.
     */
    static Lap fromJson(const QJsonObject& json);

    /**
     * Create a new Lap object from raw data
     * @param rawData The raw data counter of the new Lap
     * @return A new Lap object created from the given raw data
     */
    static Lap fromRawData(int rawData);

private:

    QTime m_time;              /** The specific lap time */
    QString m_relativeTime;    /** String representation of the relative lap time, i.e. compared to another lap */
    QString m_absoluteTime;    /** String representation of the specific (absolute) lap time */
    QString m_note;            /** Custom lap annotation */
};

Q_DECLARE_METATYPE(Lap)

#endif

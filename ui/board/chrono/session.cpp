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

#include "session.h"

#include <QJsonArray>
#include <QJsonObject>

Session::Session(int time, const QDateTime& date) :
    m_time {time},
    m_date {date}
{}

void Session::setName(const QString& name)
{
    m_name = name;
}

void Session::setNote(const QString& note)
{
    m_note = note;
}

void Session::setTime(int time)
{
    m_time = time;
}

void Session::setDate(const QDateTime& date)
{
    m_date = date;
}

void Session::setIsOutdated(bool isOutdated)
{
    m_isOutdated = isOutdated;
}

QString Session::name() const
{
    return m_name;
}

QString Session::note() const
{
    return m_note;
}

int Session::time() const
{
    return m_time;
}

QDateTime Session::date() const
{
    return m_date;
}

bool Session::isOutdated() const
{
    return m_isOutdated;
}

QVector<Lap> Session::laps() const
{
    return m_laps;
}

bool Session::isEmpty() const
{
    return m_time == 0;
}

void Session::addLap(const Lap& lap)
{
    m_laps.append(lap);
}

void Session::clear()
{
    m_time = 0;
    m_laps.clear();
}

void Session::write(QJsonObject& json) const
{
    json[QStringLiteral("name")] = m_name;
    json[QStringLiteral("note")] = m_note;
    json[QStringLiteral("time")] = m_time;
    json[QStringLiteral("date")] = m_date.toString(Qt::ISODate);

    auto laps = QJsonArray {};

    for (const auto& lap : m_laps) {
        auto object = QJsonObject {};
        lap.write(object);
        laps.append(object);
    }

    json[QStringLiteral("laps")] = laps;
}

Session Session::fromJson(const QJsonObject& json)
{
    auto session = Session {};

    session.m_name = json[QStringLiteral("name")].toString();
    session.m_note = json[QStringLiteral("note")].toString();
    session.m_time = json[QStringLiteral("time")].toInt();
    session.m_date = QDateTime::fromString(json[QStringLiteral("date")].toString(), Qt::ISODate);

    const auto laps = json[QStringLiteral("laps")].toArray();
    for (const auto& lap : laps) {
        session.addLap(Lap::fromJson(lap.toObject()));
    }

    return session;
}

bool Session::operator==(const Session& right) const
{
    return m_date == right.m_date;
}

bool Session::operator!=(const Session& right) const
{
    return !(*this == right);
}


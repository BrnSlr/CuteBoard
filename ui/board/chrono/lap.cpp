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

#include "lap.h"

#include <QJsonObject>

Lap::Lap(const QTime& lap) :
    m_time {lap}
{}

QTime Lap::time() const
{
    return m_time;
}

QTime Lap::timeTo(const Lap& lap) const
{
    if (lap.time() < m_time)
        return {0, 0};

    const auto zero = QTime {0, 0};
    return zero.addMSecs(m_time.msecsTo(lap.time()));
}

void Lap::setRelativeTime(const QString& rel)
{
    m_relativeTime = rel;
}

QString Lap::relativeTime() const
{
    return m_relativeTime;
}

void Lap::setAbsoluteTime(const QString& abs)
{
    m_absoluteTime = abs;
}

QString Lap::absoluteTime() const
{
    return m_absoluteTime;
}

void Lap::setNote(const QString& note)
{
    m_note = note;
}

QString Lap::note() const
{
    return m_note;
}

int Lap::raw() const
{
    const auto zero = QTime {0, 0};
    return zero.msecsTo(m_time);
}

void Lap::write(QJsonObject& json) const
{
    json[QStringLiteral("time")] = raw();
    json[QStringLiteral("reltime")] = m_relativeTime;
    json[QStringLiteral("abstime")] = m_absoluteTime;
    json[QStringLiteral("note")] = m_note;
}

Lap Lap::fromJson(const QJsonObject& json)
{
    auto lap = fromRawData(json[QStringLiteral("time")].toInt());
    lap.m_relativeTime = json[QStringLiteral("reltime")].toString();
    lap.m_absoluteTime = json[QStringLiteral("abstime")].toString();
    lap.m_note = json[QStringLiteral("note")].toString();

    return lap;
}

Lap Lap::fromRawData(int rawData)
{
    if (rawData < 0) {
        return Lap {};
    }

    const auto zero = QTime {0, 0};
    return Lap {zero.addMSecs(rawData)};
}

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

#include "timeformat.h"

#include <QTime>

TimeFormat::TimeFormat(bool showHours, bool showMinutes, SecondFraction fractions) :
    m_showHours {showHours},
    m_showMinutes {showMinutes},
    m_fractions {fractions}
{
    setupFormat();
}

QString TimeFormat::format(const QTime& time) const
{
    const auto h = formatHours(time);
    const auto m = formatMinutes(time);
    const auto s = formatSeconds(time);
    const auto f = formatFractions(time);

    return h + m + s + f;
}

QString TimeFormat::formatHours(const QTime& time) const
{
    if (!m_showHours) {
        return {};
    }

    return time.toString(m_hourFormat);
}

QString TimeFormat::formatMinutes(const QTime& time) const
{
    if (!m_showMinutes) {
        return {};
    }

    return time.toString(m_minFormat);
}

QString TimeFormat::formatSeconds(const QTime& time) const
{
    return time.toString(m_secFormat);
}

QString TimeFormat::formatFractions(const QTime& time) const
{
    auto fracFormat = QStringLiteral("zzz");
    auto temp = QString {};

    switch (m_fractions) {
    case UpToTenths:
        temp = time.toString(fracFormat);
        return temp.left(temp.size() - 2);
    case UpToHundredths:
        temp = time.toString(fracFormat);
        return temp.left(temp.size() - 1);
    case UpToMilliseconds:
        return time.toString(fracFormat);
    default:
        return QString();
    }
}

void TimeFormat::overrideHours()
{
    if (m_showHours)
        return;

    m_showHours = true;
    setupFormat();
}

void TimeFormat::overrideMinutes()
{
    if (m_showMinutes)
        return;

    m_showMinutes = true;
    setupFormat();
}

bool TimeFormat::hasHours() const
{
    return m_showHours;
}

bool TimeFormat::hasMinutes() const
{
    return m_showMinutes;
}

bool TimeFormat::hasFractions() const
{
    return m_fractions != NoFractions;
}

TimeFormat::SecondFraction TimeFormat::secondFractions() const
{
    return m_fractions;
}

void TimeFormat::showDividers(bool show)
{
    m_showDividers = show;
    setupFormat();
}

bool TimeFormat::operator==(const TimeFormat& right) const
{
    return m_showHours == right.m_showHours &&
            m_showMinutes == right.m_showMinutes &&
            m_fractions == right.m_fractions &&
            m_showDividers == right.m_showDividers;
}

bool TimeFormat::operator!=(const TimeFormat& right) const
{
    return !(*this == right);
}

void TimeFormat::setupFormat()
{
    if (m_showHours) {
        if (m_showDividers) {
            m_hourFormat = QStringLiteral("hh:");
        }
        else {
            m_hourFormat = QStringLiteral("hh");
        }
    }

    if (m_showMinutes) {
        if (m_showDividers) {
            m_minFormat = QStringLiteral("mm:");
        }
        else {
            m_minFormat = QStringLiteral("mm");
        }
    }

    if (m_showDividers && m_fractions != NoFractions) {
        m_secFormat = QStringLiteral("ss.");
    }
    else {
        m_secFormat = QStringLiteral("ss");
    }
}

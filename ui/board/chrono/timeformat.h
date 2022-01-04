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

#ifndef TIMEFORMAT_H
#define TIMEFORMAT_H

#include <QString>

class QTime;

/**
 * @brief A wrapper for a QTime-like format time string.
 * A TimeFormat is an abstraction for a QTime-like string used for time formats.
 * A TimeFormat can be customized using booleans in the constructor.
 * The QTime-syntax used is the following:
 * "hh:" whether to show hours (00 to 24)
 * "mm:" whether to show minutes (00 to 59)
 * "ss." whether to show seconds (00 to 59). Seconds are always showed.
 * "zzz" whether to show second fractions (tenths or hundredths or milliseconds)
 * An example of time formatted with the complete syntax might be the following: 0:05:38.582
 */
class TimeFormat
{

public:

    enum SecondFraction
    {
        UpToTenths,          /**< Second fraction is tenths of second. */
        UpToHundredths,      /**< Second fraction is hundrdths of second. */
        UpToMilliseconds,    /**< Second fraction is milliseconds. */
        NoFractions          /**< Second fraction disabled. */
    };

    explicit TimeFormat(bool showHours = false, bool showMinutes = true, SecondFraction fractions = UpToHundredths);

    /**
     * Format the given time with the current time format.
     * @param time The time to be formatted.
     * @return The time formatted as string.
     */
    QString format(const QTime& time) const;

    /**
     * Format the given time's hours with the current time format.
     * @param time The time to be formatted.
     * @return The time's hours formatted as string, or empty string if hour is not in the format.
     */
    QString formatHours(const QTime& time) const;

    /**
     * Format the given time's minutes with the current time format.
     * @param time The time to be formatted.
     * @return The time's minutes formatted as string, or empty string if minute is not in the format.
     */
    QString formatMinutes(const QTime& time) const;

    /**
     * Format the given time's seconds with the current time format.
     * @param time The time to be formatted.
     * @return The time's seconds formatted as string, or empty string if second is not in the format.
     */
    QString formatSeconds(const QTime& time) const;

    /**
     * Format the given time's second fractions with the current time format.
     * @param time The time to be formatted.
     * @return The time's second fractions formatted as string, or empty string if second fraction is not in the format.
     */
    QString formatFractions(const QTime& time) const;

    /**
     * Enable the hours in the time format.
     */
    void overrideHours();

    /**
     * Enable minutes in the time format.
     */
    void overrideMinutes();

    /**
     * Whether the hour is in the time format.
     * @return true if hour is in the format, false otherwise.
     */
    bool hasHours() const;

    /**
     * Whether the minute is in the time format.
     * @return true if minute is in the format, false otherwise.
     */
    bool hasMinutes() const;

    /**
     * Whether the second fraction is in the time format.
     * @return true if second fraction is in the format, false otherwise.
     */
    bool hasFractions() const;

    /**
     * @return The current FractionType in the time format.
     */
    SecondFraction secondFractions() const;

    /**
     * Whether to show the symbols ':' and '.' used as dividers in the time format.
     * @param show true Whether to show the dividers.
     */
    void showDividers(bool show);

    bool operator==(const TimeFormat& right) const;
    bool operator!=(const TimeFormat& right) const;

private:

    bool m_showHours;             /** Whether hour is in the internal time format */
    bool m_showMinutes;           /** Whether minute is in the internal time format */
    bool m_showDividers = true;   /** Whether to show the symbols used as dividers */

    QString m_hourFormat;         /** Hour string format */
    QString m_minFormat;          /** Minute string format */
    QString m_secFormat;          /** Secondstring format */
    SecondFraction m_fractions;   /** Second fraction internal time format */

    /**
     * Setup the format strings based on the internal formats
     */
    void setupFormat();
};

#endif

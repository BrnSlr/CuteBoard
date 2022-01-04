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

#include "digitdisplay.h"

#include <QHBoxLayout>
#include <QLabel>

DigitDisplay::DigitDisplay(QWidget *parent, Digits digits) : QWidget(parent)
{
    auto layout = new QHBoxLayout {this};
    m_leftmostDigit = new QLabel {this};
    m_centerDigit = new QLabel {this};
    m_rightmostDigit = new QLabel {this};

    m_leftmostDigit->setAlignment(Qt::AlignCenter);
    m_centerDigit->setAlignment(Qt::AlignCenter);
    m_rightmostDigit->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_leftmostDigit);
    layout->addWidget(m_centerDigit);
    layout->addWidget(m_rightmostDigit);

    setDigits(digits);
}

void DigitDisplay::setDigits(Digits digits)
{
    m_digits = digits;

    switch (m_digits) {
    case Digits::One:
        m_leftmostDigit->show();
        m_centerDigit->hide();
        m_rightmostDigit->hide();
        break;
    case Digits::Two:
        m_leftmostDigit->show();
        m_centerDigit->show();
        m_rightmostDigit->hide();
        break;
    case Digits::Three:
        m_leftmostDigit->show();
        m_centerDigit->show();
        m_rightmostDigit->show();
        break;
    default:
        m_leftmostDigit->hide();
        m_centerDigit->hide();
        m_rightmostDigit->hide();
        break;
    }
}

void DigitDisplay::showDigits(const QString& digits) const
{
    switch (m_digits) {
    case Digits::One:
        showOneDigit(digits);
        break;
    case Digits::Two:
        showTwoDigits(digits);
        break;
    case Digits::Three:
        showThreeDigits(digits);
        break;
    default:
        break;
    }
}

void DigitDisplay::setFont(const QFont& font)
{
    m_displayFont = font;

    m_leftmostDigit->setFont(m_displayFont);
    m_centerDigit->setFont(m_displayFont);
    m_rightmostDigit->setFont(m_displayFont);
}

QSize DigitDisplay::minimumSizeHint() const
{
    auto width = 0;
    auto fontMetrics = QFontMetrics {m_displayFont};

    switch (m_digits) {
    case Digits::One:
        width = fontMetrics.boundingRect(m_leftmostDigit->text()).width();
        break;
    case Digits::Two:
        width = fontMetrics.boundingRect(m_leftmostDigit->text()).width() + fontMetrics.boundingRect(m_centerDigit->text()).width();
        break;
    case Digits::Three:
        width = fontMetrics.boundingRect(m_leftmostDigit->text()).width() + fontMetrics.boundingRect(m_centerDigit->text()).width() + fontMetrics.boundingRect(m_rightmostDigit->text()).width();
        break;
    default:
        break;
    }

    width += (width * 10 / 100); // 10% used as padding between digits

    return {width, QWidget::minimumSizeHint().height()};
}

bool DigitDisplay::isValid(const QString &text) const
{
    switch (m_digits) {
    case Digits::One:
        return text.size() == 1;
    case Digits::Two:
        return text.size() == 2;
    case Digits::Three:
        return text.size() == 3;
    default:
        return false;
    }
}

void DigitDisplay::showOneDigit(const QString& digit) const
{
    if (! isValid(digit)) {
        return;
    }

    m_leftmostDigit->setText(digit.at(0));
}

void DigitDisplay::showTwoDigits(const QString& digits) const
{
    if (! isValid(digits)) {
        return;
    }

    // digits are displayed from right to left
    m_centerDigit->setText(digits.at(1));
    m_leftmostDigit->setText(digits.at(0));
}

void DigitDisplay::showThreeDigits(const QString& digits) const
{
    if (! isValid(digits)) {
        return;
    }

    // digits are displayed from right to left
    m_rightmostDigit->setText(digits.at(2));
    m_centerDigit->setText(digits.at(1));
    m_leftmostDigit->setText(digits.at(0));
}

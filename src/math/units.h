// This file is part of the SpeedCrunch project
// Copyright (C) 2015 Pol Welter <polwelter@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef UNITS_H
#define UNITS_H

#include <QHash>
#include <QMap>

class Quantity;
class Rational;

class Units
{
private:
    static void init();
    static QHash<QMap<QString, Rational>, Quantity> m_matchLookup;
    static void pushUnit(Quantity q, QString name);

public:
    static void findUnit(Quantity &q);


    // base SI units
    static const Quantity meter();
    static const Quantity second();
    static const Quantity kilogram();
    static const Quantity ampere();
    static const Quantity mole();
    static const Quantity kelvin();
    static const Quantity candela();

    static const Quantity sqmeter();
    static const Quantity cbmeter();

    // derived SI units
    static const Quantity newton();
    static const Quantity hertz();
    static const Quantity radian();
    static const Quantity steradian();
    static const Quantity pascal();
    static const Quantity joule();
    static const Quantity watt();
    static const Quantity coulomb();
    static const Quantity volt();
    static const Quantity farad();
    static const Quantity ohm();
    static const Quantity siemens();
    static const Quantity weber();
    static const Quantity tesla();
    static const Quantity henry();
    static const Quantity lumen();
    static const Quantity lux();
    static const Quantity becquerel();
    static const Quantity gray();
    static const Quantity sievert();
    static const Quantity katal();

    //SI prefixes
    static const Quantity yocto();
    static const Quantity zepto();
    static const Quantity atto();
    static const Quantity femto();
    static const Quantity pico();
    static const Quantity nano();
    static const Quantity micro();
    static const Quantity milli();
    static const Quantity centi();
    static const Quantity deci();

    static const Quantity deca();
    static const Quantity hecto();
    static const Quantity kilo();
    static const Quantity mega();
    static const Quantity giga();
    static const Quantity tera();
    static const Quantity peta();
    static const Quantity exa();
    static const Quantity zetta();
    static const Quantity yotta();
};

#endif // UNITS_H

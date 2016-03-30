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

#include "math/units.h"
#include "math/rational.h"
#include "math/quantity.h"
#include <QString>
#include <QStringList>

#define UNIT_CACHE(name, value)                \
    const Quantity Units::name()                    \
    {                                               \
        if (!m_cache.contains(#name))               \
            m_cache[#name] = value;                 \
        return (m_cache[#name]);                    \
    }

#define BASE_UNIT_CACHE(name, dimension)       \
    const Quantity Units::name()                    \
    {                                               \
        if (!m_cache.contains(#name)) {             \
            Quantity name(1);                       \
            name.modifyDimension(dimension, 1);     \
            m_cache[#name] = name;                  \
        }                                           \
        return m_cache[#name];                      \
    }



QHash<QMap<QString, Rational>, Unit> Units::m_matchLookup;
QMap<QString, Quantity> Units::m_cache;


void Units::pushUnit(Quantity q, QString name)
{
    q.cleanDimension();
    Unit u(name, q);
    m_matchLookup.insert(q.getDimension(), u);
}

unsigned int qHash(QMap<QString, Rational> dimension)
{
    QStringList keyList(dimension.keys());
    QString blob("");
    keyList.sort();
    for (int i=0; i<keyList.size(); ++i) {
        keyList[i].append(dimension[keyList[i]].toString());
        blob.append(keyList[i]);
    }
    return qHash(blob);
}


/*
 * initialize the lookup table for automatic matching
 */
void Units::initTable()
{
    m_matchLookup.clear();
    pushUnit(joule(), "newton meter");                          // energy or torque
    pushUnit(newton(), "newton");                               // force
    pushUnit(watt(), "watt");                                   // power
    pushUnit(pascal(), "pascal");                               // pressure or energy density
    pushUnit(coulomb(), "coulomb");                             // charge
    pushUnit(volt(), "volt");                                   // electrical potential
    pushUnit(ohm(), "ohm");                                     // el. resistance
    pushUnit(siemens(), "siemens");                             // el. conductance
    pushUnit(ohm()*meter(), "ohm meter");                       // el. resistivity
    pushUnit(siemens()/meter(), "siemens/meter");               // el. conductivity
    pushUnit(siemens()/meter()/mole(), "siemens/(meter mole)"); // molar conductivity
    pushUnit(farad(), "farad");                                 // capacitance
    pushUnit(tesla(), "tesla");                                 // magnetic flux density
    pushUnit(weber(), "weber");                                 // magnetic flux
    pushUnit(henry(), "henry");                                 // inductance
    pushUnit(coulomb()/cbmeter(), "coulomb/meter³");            // electric charge density
    pushUnit(coulomb()/sqmeter(), "coulomb/meter²");            // surface charge density or el. flux
    pushUnit(coulomb()/kilogram(), "coulomb/kilogram");         // exposure
    pushUnit(farad()/meter(), "farad/meter");                   // permittivity
    pushUnit(henry()/meter(), "henry/meter");                   // permeability
    pushUnit(joule()/kilogram()/kelvin(),"joule/(kilogram kelvin)");    // specific heat capacity
    pushUnit(joule()/mole()/kelvin(), "joule/(mole kelvin");            // molar heat capacity
    pushUnit(mole()/second()/cbmeter(), "mole/(second meter³)");        // catalytic activity
    pushUnit(newton()/meter(), "newton/meter");                 // surface tension
    pushUnit(pascal()*second(), "pascal second");               // dynamic viscosity
    pushUnit(volt()/meter(), "volt/meter");                     // el. field
    pushUnit(watt()/meter()/kelvin(), "watt/(meter kelvin)");   // thermal conductivity
    pushUnit(watt()/sqmeter(), "watt/meter²");                  // heat flux
    pushUnit(joule()/kelvin(), "joule/kelvin");                 // entropy or heat capacity
    pushUnit(joule()/kilogram(), "joule/kilogram");             // specific energy
}



void Units::findUnit(Quantity & q)
{
    QString unit_name = "";
    CNumber unit(1);
    q.cleanDimension();

    if (m_matchLookup.isEmpty())
        initTable();

    /*
     *  match derived units
     */
    if (m_matchLookup.contains(q.getDimension())) {
        Unit temp(m_matchLookup[q.getDimension()]);
        q.setDisplayUnit(temp.value.numericValue(), temp.name);
    }
    else
    {
        /*
         *  autogenerate unit name (product of base units)
         */
        QMap<QString, Rational> dimension = q.getDimension();
        QMap<QString, Rational>::const_iterator i = dimension.constBegin();
        while (i != dimension.constEnd()) {
            QString exponent = i.value().toString();
            if(exponent.contains('/'))
                exponent = "^(" + exponent+')';
            else if(exponent == "1")
                exponent = "";
            else
                exponent = '^' + exponent;

            if(exponent == QLatin1String("^0")) exponent = QString::fromUtf8("⁰");
            else if (exponent == QLatin1String("^2")) exponent = QString::fromUtf8("²");
            else if (exponent == QLatin1String("^3")) exponent = QString::fromUtf8("³");
            else if (exponent == QLatin1String("^4")) exponent = QString::fromUtf8("⁴") ;
            else if (exponent == QLatin1String("^5")) exponent = QString::fromUtf8("⁵") ;
            else if (exponent == QLatin1String("^6")) exponent = QString::fromUtf8("⁶") ;
            else if (exponent == QLatin1String("^7")) exponent = QString::fromUtf8("⁷") ;
            else if (exponent == QLatin1String("^8")) exponent = QString::fromUtf8("⁸") ;
            else if (exponent == QLatin1String("^9")) exponent = QString::fromUtf8("⁹") ;



            // TODO: replace this with a lookup to a repository
            if (i.key() == "length") {
                unit_name += " meter";
            } else if (i.key() == "time") {
                unit_name += " second";
            } else if (i.key() == "mass") {
                unit_name += " kilogram";
            } else if (i.key() == "el. current") {
                unit_name += " ampere";
            } else if (i.key() == "amount") {
                unit_name += " mole";
            } else if (i.key() == "luminous intensity") {
                unit_name += " candela";
            } else if (i.key() == "temperature") {
                unit_name += " kelvin";
            } else if (i.key() == "information") {
                unit_name += " bit";
            } else {
                unit_name += " " + i.key(); // fall back to the dimension name
            }
            unit_name += exponent;
            ++i;
        }
        q.setDisplayUnit(unit, unit_name.trimmed());
    }
}

#define ADD_UNIT(name)  result.append(Unit(#name, name()));


// This list contains the units that wil be set as builtin variables by the evaluator
const QList<Unit> Units::getList()
{
    QList<Unit> result;


    ADD_UNIT(meter);
    ADD_UNIT(second);
    ADD_UNIT(kilogram);
    ADD_UNIT(ampere);
    ADD_UNIT(mole);
    ADD_UNIT(candela);
    ADD_UNIT(kelvin);
    ADD_UNIT(bit);

    ADD_UNIT(yocto);
    ADD_UNIT(zepto);
    ADD_UNIT(atto);
    ADD_UNIT(femto);
    ADD_UNIT(pico);
    ADD_UNIT(nano);
    ADD_UNIT(micro);
    ADD_UNIT(milli);
    ADD_UNIT(centi);
    ADD_UNIT(deci);
    ADD_UNIT(deca);
    ADD_UNIT(hecto);
    ADD_UNIT(kilo);
    ADD_UNIT(mega);
    ADD_UNIT(giga);
    ADD_UNIT(tera);
    ADD_UNIT(peta);
    ADD_UNIT(exa);
    ADD_UNIT(zetta);
    ADD_UNIT(yotta);
    ADD_UNIT(kibi);
    ADD_UNIT(mebi);
    ADD_UNIT(gibi);
    ADD_UNIT(tebi);
    ADD_UNIT(pebi);
    ADD_UNIT(exbi);
    ADD_UNIT(zebi);
    ADD_UNIT(yobi);

    //ADD_UNIT(radian);
    //ADD_UNIT(steradian);
    ADD_UNIT(sqmeter);
    ADD_UNIT(cbmeter);
    ADD_UNIT(newton);
    ADD_UNIT(hertz);
    ADD_UNIT(joule);
    ADD_UNIT(watt);
    ADD_UNIT(pascal);
    ADD_UNIT(coulomb);
    ADD_UNIT(volt);
    ADD_UNIT(ohm);
    ADD_UNIT(farad);
    ADD_UNIT(tesla);
    ADD_UNIT(weber);
    ADD_UNIT(henry);
    ADD_UNIT(siemens);
    //ADD_UNIT(lumen);
    //ADD_UNIT(lux);
    ADD_UNIT(becquerel);
    ADD_UNIT(gray);
    ADD_UNIT(sievert);
    ADD_UNIT(katal);

    return result;
}

BASE_UNIT_CACHE(meter,     "length")
BASE_UNIT_CACHE(second,    "time")
BASE_UNIT_CACHE(kilogram,  "mass")
BASE_UNIT_CACHE(ampere,    "el. currrent")
BASE_UNIT_CACHE(mole,      "amount")
BASE_UNIT_CACHE(kelvin,    "temperature")
BASE_UNIT_CACHE(candela,   "luminous intensity")
BASE_UNIT_CACHE(bit,       "information")


UNIT_CACHE(yocto, HNumber("1e-24"))
UNIT_CACHE(zepto, HNumber("1e-21"))
UNIT_CACHE(atto,  HNumber("1e-18"))
UNIT_CACHE(femto, HNumber("1e-15"))
UNIT_CACHE(pico,  HNumber("1e-12"))
UNIT_CACHE(nano,  HNumber("1e-9"))
UNIT_CACHE(micro, HNumber("1e-6"))
UNIT_CACHE(milli, HNumber("1e-3"))
UNIT_CACHE(centi, HNumber("1e-2"))
UNIT_CACHE(deci,  HNumber("1e-1"))

UNIT_CACHE(deca,  HNumber("1e1"))
UNIT_CACHE(hecto, HNumber("1e2"))
UNIT_CACHE(kilo,  HNumber("1e3"))
UNIT_CACHE(mega,  HNumber("1e6"))
UNIT_CACHE(giga,  HNumber("1e9"))
UNIT_CACHE(tera,  HNumber("1e12"))
UNIT_CACHE(peta,  HNumber("1e15"))
UNIT_CACHE(exa,   HNumber("1e18"))
UNIT_CACHE(zetta, HNumber("1e21"))
UNIT_CACHE(yotta, HNumber("1e24"))

UNIT_CACHE(kibi,  HNumber("1024"))
UNIT_CACHE(mebi,  kibi()*kibi())
UNIT_CACHE(gibi,  kibi()*mebi())
UNIT_CACHE(tebi,  kibi()*gibi())
UNIT_CACHE(pebi,  kibi()*tebi())
UNIT_CACHE(exbi,  kibi()*pebi())
UNIT_CACHE(zebi,  kibi()*exbi())
UNIT_CACHE(yobi,  kibi()*zebi())






//We want to avoid caching angle units. Their value should depend on the angle mode.
const Quantity Units::radian()
{
    return Quantity(1);
}
const Quantity Units::steradian()
{
    return Quantity(1);
}
const Quantity Units::lumen()
{
    return candela()*steradian();
}
const Quantity Units::lux()
{
    return lumen()/sqmeter();
}


UNIT_CACHE(newton,  meter()*kilogram()/second()/second())
UNIT_CACHE(hertz,   Quantity(1)/second())
UNIT_CACHE(pascal,  newton()/sqmeter())
UNIT_CACHE(joule,   newton()*meter())
UNIT_CACHE(watt,    joule()/second())
UNIT_CACHE(coulomb, ampere()*second())
UNIT_CACHE(volt,    watt()/ampere())
UNIT_CACHE(farad,   coulomb()/volt())
UNIT_CACHE(ohm,     volt()/ampere())
UNIT_CACHE(siemens, ampere()/volt())
UNIT_CACHE(weber,   volt()*second())
UNIT_CACHE(tesla,   weber()/sqmeter())
UNIT_CACHE(henry,   weber()/ampere())
UNIT_CACHE(becquerel, Quantity(1)/second())
UNIT_CACHE(gray,    joule()/kilogram())
UNIT_CACHE(sievert,    joule()/kilogram())
UNIT_CACHE(katal, mole()/second())

UNIT_CACHE(sqmeter, meter()*meter())
UNIT_CACHE(cbmeter, sqmeter()*meter())

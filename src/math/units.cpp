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

QHash<QMap<QString, Rational>, Quantity> Units::m_matchLookup;


void Units::pushUnit(Quantity q, QString name)
{
    q.cleanDimension();
    q.setDisplayUnit(CNumber(1), name);         //value of the unit is irrelevant here
    m_matchLookup.insert(q.getDimension(), q);
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
void Units::init()
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
        init();

    /*
     *  match derived units
     */
    if (m_matchLookup.contains(q.getDimension())) {
        Quantity temp(m_matchLookup[q.getDimension()]);
        q.setDisplayUnit(temp.numericValue(), temp.unitName());
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
            } else {
                unit_name += " " + i.key(); // fall back to the dimension name
            }
            unit_name += exponent;
            ++i;
        }
        q.setDisplayUnit(unit, unit_name.trimmed());
    }
}

const Quantity Units::meter()
{
    Quantity meter(1);
    meter.modifyDimension("length", 1);
    return meter;
}

const Quantity Units::second()
{
    Quantity second(1);
    second.modifyDimension("time", 1);
    return second;
}

const Quantity Units::kilogram()
{
    Quantity kilogram(1);
    kilogram.modifyDimension("mass", 1);
    return kilogram;
}

const Quantity Units::ampere()
{
    Quantity ampere(1);
    ampere.modifyDimension("el. current", 1);
    return ampere;
}

const Quantity Units::mole()
{
    Quantity mole(1);
    mole.modifyDimension("amount", 1);
    return mole;
}

const Quantity Units::kelvin()
{
    Quantity kelvin(1);
    kelvin.modifyDimension("temperature", 1);
    return kelvin;
}

const Quantity Units::candela()
{
    Quantity candela(1);
    candela.modifyDimension("luminous intensity", 1);
    return candela;
}

const Quantity Units::newton()
{
    return meter()*kilogram()/second()/second();
}

const Quantity Units::hertz()
{
    return Quantity(1)/second();
}

const Quantity Units::radian()
{
    return Quantity(1);
}

const Quantity Units::steradian()
{
    return Quantity(1);
}

const Quantity Units::pascal()
{
    return newton()/sqmeter();
}

const Quantity Units::joule()
{
    return newton()*meter();
}

const Quantity Units::watt()
{
    return joule()/second();
}

const Quantity Units::coulomb()
{
    return ampere()*second();
}

const Quantity Units::volt()
{
    return watt()/ampere();
}

const Quantity Units::farad()
{
    return coulomb()/volt();
}

const Quantity Units::ohm()
{
    return volt()/ampere();
}

const Quantity Units::siemens()
{
    return ampere()/volt();
}

const Quantity Units::weber()
{
    return volt()*second();
}

const Quantity Units::tesla()
{
    return weber()/sqmeter();
}

const Quantity Units::henry()
{
    return weber()/ampere();
}

const Quantity Units::lumen()
{
    return candela()*steradian();
}

const Quantity Units::lux()
{
    return lumen()/sqmeter();
}

const Quantity Units::becquerel()
{
    return Quantity(1)/second();
}

const Quantity Units::gray()
{
    return joule()/kilogram();
}

const Quantity Units::sievert()
{
    return joule()/kilogram();
}

const Quantity Units::katal()
{
    return mole()/second();
}

const Quantity Units::sqmeter()
{
    return meter()*meter();
}

const Quantity Units::cbmeter()
{
    return sqmeter()*meter();
}

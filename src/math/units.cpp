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
#include <QMap>


#define MATCH_UNIT(val, name)   if(q.sameDimension(val)) { \
                                    q.setDisplayUnit((val).numericValue(), (name));\
                                }

void Units::findUnit(Quantity & q)
{
    QString unit_name = "";
    CNumber unit(1);
    q.cleanDimension();

    /*
     *  match derived units
     *  TODO: Store these units in a data structure to match the dimension vector more efficiently
     */
    MATCH_UNIT(joule(), "newton meter")                             // energy or torque
    else MATCH_UNIT(newton(), "newton")                             // force
    else MATCH_UNIT(watt(), "watt")                                 // power
    else MATCH_UNIT(pascal(), "pascal")                             // pressure or energy density
    else MATCH_UNIT(coulomb(), "coulomb")                           // charge
    else MATCH_UNIT(volt(), "volt")                                 // electrical potential
    else MATCH_UNIT(ohm(), "ohm")                                   // el. resistance
    else MATCH_UNIT(siemens(), "siemens")                           // el. conductance
    else MATCH_UNIT(ohm()*meter(), "ohm meter")                     // el. resistivity
    else MATCH_UNIT(siemens()/meter(), "siemens/meter")             // el. conductivity
    else MATCH_UNIT(siemens()/meter()/mole(), "siemens/(meter mole)")         // molar conductivity
    else MATCH_UNIT(farad(), "farad")                               // capacitance
    else MATCH_UNIT(tesla(), "tesla")                               // magnetic flux density
    else MATCH_UNIT(weber(), "weber")                               // magnetic flux
    else MATCH_UNIT(henry(), "henry")                               // inductance
    else MATCH_UNIT(coulomb()/cbmeter(), "coulomb/meter³")          // electric charge density
    else MATCH_UNIT(coulomb()/sqmeter(), "coulomb/meter²")          // surface charge density or el. flux
    else MATCH_UNIT(coulomb()/kilogram(), "coulomb/kilogram")       // exposure
    else MATCH_UNIT(farad()/meter(), "farad/meter")                 // permittivity
    else MATCH_UNIT(henry()/meter(), "henry/meter")                 // permeability
    else MATCH_UNIT(joule()/kilogram()/kelvin(),"joule/(kilogram kelvin)")    // specific heat capacity
    else MATCH_UNIT(joule()/mole()/kelvin(), "joule/(mole kelvin")  // molar heat capacity
    else MATCH_UNIT(mole()/second()/cbmeter(), "mole/(second meter³)")        // catalytic activity
    else MATCH_UNIT(newton()/meter(), "newton/meter")               // surface tension
    else MATCH_UNIT(pascal()*second(), "pascal second")             // dynamic viscosity
    else MATCH_UNIT(volt()/meter(), "volt/meter")                   // el. field
    else MATCH_UNIT(watt()/meter()/kelvin(), "watt/(meter kelvin)") // thermal conductivity
    else MATCH_UNIT(watt()/sqmeter(), "watt/meter²")                // heat flux
    else MATCH_UNIT(joule()/kelvin(), "joule/kelvin")               // entropy or heat capacity
    else MATCH_UNIT(joule()/kilogram(), "joule/kilogram")           // specific energy
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
            else if(exponent == QLatin1String("^2")) exponent = QString::fromUtf8("²");
            else if(exponent == QLatin1String("^3")) exponent = QString::fromUtf8("³");
            else if(exponent == QLatin1String("^4")) exponent = QString::fromUtf8("⁴") ;
            else if(exponent == QLatin1String("^5")) exponent = QString::fromUtf8("⁵") ;
            else if(exponent == QLatin1String("^6")) exponent = QString::fromUtf8("⁶") ;
            else if(exponent == QLatin1String("^7")) exponent = QString::fromUtf8("⁷") ;
            else if(exponent == QLatin1String("^8")) exponent = QString::fromUtf8("⁸") ;
            else if(exponent == QLatin1String("^9")) exponent = QString::fromUtf8("⁹") ;



            // TODO: replace this with a lookup to a repository
            if(i.key() == "length") {
                unit_name += " meter";
            } else if(i.key() == "time") {
                unit_name += " second";
            } else if(i.key() == "mass") {
                unit_name += " kilogram";
            } else if(i.key() == "el. current") {
                unit_name += " ampere";
            } else if(i.key() == "amount") {
                unit_name += " mole";
            } else if(i.key() == "luminous intensity") {
                unit_name += " candela";
            } else if(i.key() == "temperature") {
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
    return newton()/meter()/meter();
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
    return weber()/meter()/meter();
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
    return lumen()/meter()/meter();
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
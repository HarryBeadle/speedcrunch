#include "math/units.h"
#include "math/cmath.h"
#include "math/rational.h"
#include <QString>
#include <QMap>

void Units::findUnit(CNumber & number)
{
    QString unit_name = "";
    CNumber unit(number);
    unit.clearDimension();
    unit = number/unit;
    number.cleanDimension();

    /*
     *  match derived units
     */
    if(number.sameDimension(joule())) {                             // energy or torque
        number.setDisplayUnit(joule(), "newton meter");
    }
    else if(number.sameDimension(newton())) {                       // force
        number.setDisplayUnit(newton(), "newton");
    }
    else if(number.sameDimension(watt())) {                         // power
        number.setDisplayUnit(watt(), "watt");
    }
    else if(number.sameDimension(pascal())) {                       // pressure or energy density
        number.setDisplayUnit(pascal(), "pascal");
    }
    else if(number.sameDimension(coulomb())) {                      // charge
        number.setDisplayUnit(coulomb(), "coulomb");
    }
    else if(number.sameDimension(volt())) {                         // electrical potential
        number.setDisplayUnit(volt(), "volt");
    }
    else if(number.sameDimension(ohm())) {                          // el. resistance
        number.setDisplayUnit(ohm(), "ohm");
    }
    else if(number.sameDimension(siemens())) {                      // el. conductance
        number.setDisplayUnit(siemens(), "siemens");
    }
    else if(number.sameDimension(ohm()*meter())) {                  // el. resistivity
        number.setDisplayUnit(ohm()*meter(), "ohm meter");
    }
    else if(number.sameDimension(siemens()/meter())) {              // el. conductivity
        number.setDisplayUnit(siemens()/meter(), "siemens/meter");
    }
    else if(number.sameDimension(siemens()/meter()/mole())) {       // molar conductivity
        number.setDisplayUnit(siemens()/meter()/mole(),
                              "siemens/(meter mole)");
    }
    else if(number.sameDimension(farad())) {                        // capacitance
        number.setDisplayUnit(farad(), "farad");
    }
    else if(number.sameDimension(tesla())) {                        // magnetic flux density
        number.setDisplayUnit(tesla(), "tesla");
    }
    else if(number.sameDimension(weber())) {                        // magnetic flux
        number.setDisplayUnit(weber(), "weber");
    }
    else if(number.sameDimension(henry())) {                        // inductance
        number.setDisplayUnit(henry(), "henry");
    }
    else if(number.sameDimension(coulomb()/cbmeter())) {            // electric charge density
        number.setDisplayUnit(coulomb()/cbmeter(),
                              QString::fromUtf8("coulomb/meter³"));
    }
    else if(number.sameDimension(coulomb()/sqmeter())) {            // surface charge density or el. flux
        number.setDisplayUnit(coulomb()/sqmeter(),
                              QString::fromUtf8("coulomb/meter²"));
    }
    else if(number.sameDimension(coulomb()/kilogram())) {           // exposure
        number.setDisplayUnit(coulomb()/kilogram(), "coulomb/kilogram");
    }
    else if(number.sameDimension(farad()/meter())) {                // permittivity
        number.setDisplayUnit(farad()/meter(), "farad/meter");
    }
    else if(number.sameDimension(henry()/meter())) {                // permeability
        number.setDisplayUnit(henry()/meter(), "henry/meter");
    }
    else if(number.sameDimension(joule()/kilogram()/kelvin())) {    // specific heat capacity
        number.setDisplayUnit(joule()/kilogram()/kelvin(),
                              "joule/(kilogram kelvin)");
    }
    else if(number.sameDimension(joule()/mole()/kelvin())) {        // molar heat capacity
        number.setDisplayUnit(joule()/mole()/kelvin(),
                              "joule/(mole kelvin)");
    }
    else if(number.sameDimension(mole()/second()/cbmeter())) {      // catalytic activity
        number.setDisplayUnit(mole()/second()/cbmeter(),
                              QString::fromUtf8("mole/(second meter³)"));
    }
    else if(number.sameDimension(newton()/meter())) {               // surface tension
        number.setDisplayUnit(newton()/meter(), "newton/meter");
    }
    else if(number.sameDimension(pascal()*second())) {              // dynamic viscosity
        number.setDisplayUnit(pascal()*second(), "pascal second");
    }
    else if(number.sameDimension(volt()/meter())) {                 // el. field
        number.setDisplayUnit(volt()/meter(), "volt/meter");
    }
    else if(number.sameDimension(watt()/meter()/kelvin())) {        // thermal conductivity
        number.setDisplayUnit(watt()/meter()/kelvin(), "watt/(meter kelvin)");
    }
    else if(number.sameDimension(watt()/sqmeter())) {               // heat flux
        number.setDisplayUnit(watt()/sqmeter(), "watt/meter²");
    }
    else if(number.sameDimension(joule()/kelvin())) {               // entropy
        number.setDisplayUnit(joule()/kelvin(), "joule/kelvin");
    }
    else if(number.sameDimension(joule()/kilogram())) {             // specific energy
        number.setDisplayUnit(joule()/kilogram(), "joule/kilogram");
    }
    else if(number.sameDimension(joule()/kilogram())) {             // specific energy
        number.setDisplayUnit(joule()/kilogram(), "joule/kilogram");

    }
    else
    {
        /*
         *  autogenerate unit name (product of base units)
         */
        QMap<QString, Rational> dimension = number.getDimension();
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
                unit_name += "meter";
            } else if(i.key() == "time") {
                unit_name += " second";
            } else if(i.key() == "mass") {
                unit_name += " kilogram";
            } else if(i.key() == "el. current") {
                unit_name += " ampere";
            } else if(i.key() == "amount") {
                unit_name += " mole";
            } else if(i.key() == "luminous intensity") {
                unit_name += " canedela";
            } else if(i.key() == "temperature") {
                unit_name += " kelvin";
            } else {
                unit_name += " " + i.key(); // fall back to the dimension name
            }
            unit_name += exponent;
            ++i;
        }
        number.setDisplayUnit(unit, unit_name);
    }
}

const CNumber Units::meter()
{
    CNumber meter(1);
    meter.modifyDimension("length", 1);
    return meter;
}

const CNumber Units::second()
{
    CNumber second(1);
    second.modifyDimension("time", 1);
    return second;
}

const CNumber Units::kilogram()
{
    CNumber kilogram(1);
    kilogram.modifyDimension("mass", 1);
    return kilogram;
}

const CNumber Units::ampere()
{
    CNumber ampere(1);
    ampere.modifyDimension("el. current", 1);
    return ampere;
}

const CNumber Units::mole()
{
    CNumber mole(1);
    mole.modifyDimension("amount", 1);
    return mole;
}

const CNumber Units::kelvin()
{
    CNumber kelvin(1);
    kelvin.modifyDimension("temperature", 1);
    return kelvin;
}

const CNumber Units::candela()
{
    CNumber candela(1);
    candela.modifyDimension("luminous intensity", 1);
    return candela;
}

const CNumber Units::newton()
{
    return meter()*kilogram()/second()/second();
}

const CNumber Units::hertz()
{
    return CNumber(1)/second();
}

const CNumber Units::radian()
{
    return CNumber(1);
}

const CNumber Units::steradian()
{
    return CNumber(1);
}

const CNumber Units::pascal()
{
    return newton()/meter()/meter();
}

const CNumber Units::joule()
{
    return newton()*meter();
}

const CNumber Units::watt()
{
    return joule()/second();
}

const CNumber Units::coulomb()
{
    return ampere()*second();
}

const CNumber Units::volt()
{
    return watt()/ampere();
}

const CNumber Units::farad()
{
    return coulomb()/volt();
}

const CNumber Units::ohm()
{
    return volt()/ampere();
}

const CNumber Units::siemens()
{
    return ampere()/volt();
}

const CNumber Units::weber()
{
    return volt()*second();
}

const CNumber Units::tesla()
{
    return weber()/meter()/meter();
}

const CNumber Units::henry()
{
    return weber()/ampere();
}

const CNumber Units::lumen()
{
    return candela()*steradian();
}

const CNumber Units::lux()
{
    return lumen()/meter()/meter();
}

const CNumber Units::becquerel()
{
    return CNumber(1)/second();
}

const CNumber Units::gray()
{
    return joule()/kilogram();
}

const CNumber Units::sievert()
{
    return joule()/kilogram();
}

const CNumber Units::katal()
{
    return mole()/second();
}

const CNumber Units::sqmeter()
{
    return meter()*meter();
}

const CNumber Units::cbmeter()
{
    return sqmeter()*meter();
}

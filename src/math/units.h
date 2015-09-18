#ifndef UNITS_H
#define UNITS_H

#include "math/cmath.h"


class Units
{
public:
    static void findUnit(CNumber &number);

    static const CNumber meter();
    static const CNumber second();
    static const CNumber kilogram();
    static const CNumber ampere();
    static const CNumber mole();
    static const CNumber kelvin();
    static const CNumber candela();

    static const CNumber sqmeter();
    static const CNumber cbmeter();

    static const CNumber newton();
    static const CNumber hertz();
    static const CNumber radian();
    static const CNumber steradian();
    static const CNumber pascal();
    static const CNumber joule();
    static const CNumber watt();
    static const CNumber coulomb();
    static const CNumber volt();
    static const CNumber farad();
    static const CNumber ohm();
    static const CNumber siemens();
    static const CNumber weber();
    static const CNumber tesla();
    static const CNumber henry();
    static const CNumber lumen();
    static const CNumber lux();
    static const CNumber becquerel();
    static const CNumber gray();
    static const CNumber sievert();
    static const CNumber katal();


    static const CNumber yocto();
    static const CNumber zepto();
    static const CNumber atto();
    static const CNumber femto();
    static const CNumber pico();
    static const CNumber nano();
    static const CNumber micro();
    static const CNumber milli();
    static const CNumber centi();
    static const CNumber deci();

    static const CNumber deca();
    static const CNumber hecto();
    static const CNumber kilo();
    static const CNumber mega();
    static const CNumber giga();
    static const CNumber tera();
    static const CNumber peta();
    static const CNumber exa();
    static const CNumber zetta();
    static const CNumber yotta();
};

#endif // UNITS_H

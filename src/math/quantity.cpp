// quantity.cpp
// Support for units and dimensions
//
// This file is part of the SpeedCrunch project
// Copyright (C) 2016 Pol Welter.
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


#include "math/quantity.h"
#include "math/rational.h"

#include <QStringList>

#define RATIONAL_TOL HNumber("1e-20")

#define ENSURE_DIMENSIONLESS(x)                 \
    if(!(x).isDimensionless())                  \
        {return DMath::nan(InvalidDimension);}


#define ENSURE_SAME_DIMENSION(x, y)                \
    if((!(x).sameDimension(y)))                    \
        {return DMath::nan(DimensionMismatch);}

Quantity operator-(const Quantity & q)
{
    Quantity res(q);
    res.m_numericValue = - res.m_numericValue;
    return res;
}

Quantity operator-(const Quantity & a, const Quantity & b)
{
    Quantity res(a);
    if(!a.sameDimension(b))
        return DMath::nan(DimensionMismatch);
    res.m_numericValue -= b.m_numericValue;
    return res;
}

bool operator>(const Quantity &l, const Quantity &r)
{
    if(l.sameDimension(r))
        return l.m_numericValue > r.m_numericValue;
    return false;
}

bool operator<(const Quantity &l, const Quantity &r)
{
    if(l.sameDimension(r))
        return l.m_numericValue < r.m_numericValue;
    return false;
}

bool operator>=(const Quantity &l, const Quantity &r)
{
    if(l.sameDimension(r))
        return l.m_numericValue >= r.m_numericValue;
    return false;
}

bool operator<=(const Quantity &l, const Quantity &r)
{
    if(l.sameDimension(r))
        return l.m_numericValue <= r.m_numericValue;
    return false;
}

bool operator==(const Quantity &l, const Quantity &r)
{
    if(l.sameDimension(r))
        return l.m_numericValue == r.m_numericValue;
    return false;
}


// returns TRUE upon dimension mismatch
bool operator!=(const Quantity &l, const Quantity &r)
{
    if(l.sameDimension(r))
        return l.m_numericValue != r.m_numericValue;
    return true;
}

Quantity::Quantity()
    : m_numericValue(0), m_unit(NULL), m_unitName("")
{
}

Quantity::Quantity(const Quantity & other)
    : m_numericValue(other.m_numericValue),
      m_dimension(other.m_dimension),
      m_unitName(other.m_unitName)
{
    if(other.hasUnit())
        this->m_unit = new CNumber(other.getUnit());
    cleanDimension();
}

Quantity::Quantity(int i) : Quantity(CNumber(i))
{
}

Quantity::Quantity(const HNumber & h): Quantity(CNumber(h))
{
}

Quantity::Quantity(const CNumber & c) : Quantity()
{
    this->m_numericValue = c;
}

Quantity::~Quantity()
{
    delete m_unit;
}

bool Quantity::isNan() const
{
    return m_numericValue.isNan();
}

bool Quantity::isZero() const
{
    return m_numericValue.isZero();
}

bool Quantity::isReal() const
{
    return m_numericValue.isReal();
}

bool Quantity::isPositive() const
{
    return m_numericValue.isPositive();
}

bool Quantity::isNegative() const
{
    return m_numericValue.isNegative();
}

bool Quantity::isInteger() const
{
    return (!this->hasDimension() && !this->hasUnit()) && m_numericValue.isInteger();
}

bool Quantity::hasUnit() const
{
    return this->m_unit != NULL;
}

CNumber Quantity::getUnit() const
{
    if(this->hasUnit())
        return CNumber(*(this->m_unit));
    return CNumber(1);
}

QString Quantity::getUnitName() const
{
    if(this->hasUnit())
        return m_unitName;
    return "";
}

Quantity &Quantity::setDisplayUnit(const CNumber unit, const QString &name)
{
    if(unit.isNan())
        *this = DMath::nan(InvalidDimension);
    else
    {
        stripUnits();
        m_unit = new CNumber(unit);
        m_unitName = name;
    }
    return *this;
}

void Quantity::stripUnits()
{
    delete m_unit;
    m_unitName = "";
}

bool Quantity::hasDimension() const
{
    return !this->m_dimension.empty();
}

/*
 * Unlike hasDimension(), this does a clean up first, i.e. it
 * checks for redundant exponents.
 */
bool Quantity::isDimensionless() const
{
    Quantity temp(*this);
    temp.cleanDimension();
    return temp.hasDimension();
}

QMap<QString, Rational> Quantity::getDimension() const
{
    Quantity temp(*this);
    temp.cleanDimension();
    return temp.m_dimension;
}

void Quantity::modifyDimension(const QString &key, const Rational &exponent)
{
    m_dimension.insert(key, exponent);
}

void Quantity::copyDimension(const Quantity &other)
{
    clearDimension();
    this->m_dimension = other.m_dimension;
}

void Quantity::clearDimension()
{
    this->m_dimension.clear();
}


/*
 * Note: does not clean the dimension vector first.
 * The calling function must do so on its own.
 */
bool Quantity::sameDimension(const Quantity &other) const
{
    return this->m_dimension == other.m_dimension;
}

void Quantity::cleanDimension()
{
    QMap<QString, Rational>::iterator i = m_dimension.begin();
    while (i != m_dimension.end()) {
        if(!i.value().isZero())
            m_dimension.erase(i);
        ++i;
    }
}

void Quantity::serialize(QJsonObject &json) const
{
    QJsonObject nom_json;
    m_numericValue.serialize(nom_json);
    json["nominal value"] = nom_json;

    if(hasDimension()) {
        QJsonObject dim_json;
        QMap<QString, Rational>::const_iterator i = m_dimension.constBegin();
        while (i != m_dimension.constEnd()) {
            const Rational & exp = i.value();
            const QString & name = i.key();
            dim_json[name] = exp.toString();
            ++i;
        }
        json["dimension"] = dim_json;
    }

    if(hasUnit())
    {
        QJsonObject unit_json;
        m_unit->serialize(unit_json);
        json["unit"] = unit_json;
    }
}

Quantity Quantity::deSerialize(const QJsonObject &json)
{
    Quantity result;
    QJsonObject nom_json = json["numeric value"].toObject();
    result.m_numericValue.deSerialize(nom_json);

    if(json.contains("unit")) {
        QJsonObject unit_json = json["unit"].toObject();
        result.m_unit->deSerialize(unit_json);
    }

    if(json.contains("dimension")) {
        QJsonObject dim_json = json["dimension"].toObject();
        for(int i=0; i<dim_json.count(); ++i) {
            QString key = dim_json.keys()[i];
            Rational val(dim_json[key].toString());
            result.modifyDimension(key, val);
        }
    }
    return result;
}

Error Quantity::error() const
{
    return m_numericValue.error();
}

Quantity &Quantity::operator=(const Quantity &other)
{
    m_numericValue = other.m_numericValue;
    m_dimension = other.m_dimension;
    stripUnits();
    if(other.hasUnit())
    {
        m_unit = new CNumber(*other.m_unit);
        m_unitName = other.m_unitName;
    }
    cleanDimension();
    return *this;
}

Quantity Quantity::operator+(const Quantity & other) const
{
    if(!this->sameDimension(other))
        return DMath::nan(DimensionMismatch);
    Quantity result(*this);
    result.m_numericValue += other.m_numericValue;
    return result;
}

Quantity &Quantity::operator+=(const Quantity & other)
{
    if(!this->sameDimension(other))
        *this = DMath::nan(DimensionMismatch);
    else
        this->m_numericValue += other.m_numericValue;
    return *this;
}

Quantity &Quantity::operator-=(const Quantity & other)
{
    return operator=(*this - other);
}

Quantity Quantity::operator*(const Quantity & other) const
{
    Quantity result(*this);
    result.m_numericValue *= other.m_numericValue;
    QMap<QString, Rational>::const_iterator i = other.m_dimension.constBegin();
    while (i != other.m_dimension.constEnd()) {
        const Rational & exp = i.value();
        const QString & name = i.key();
        if(!result.m_dimension.contains(name))
            result.m_dimension[name] = Rational(0);
        result.m_dimension[name] += exp;
        ++i;
    }
    result.cleanDimension();
    return result;
}

Quantity Quantity::operator*(const CNumber & other) const
{
    Quantity result(*this);
    result.m_numericValue *= other;
    return result;
}

Quantity Quantity::operator*(const HNumber & other) const
{
    return operator*(CNumber(other));
}

Quantity Quantity::operator/(const Quantity & other) const
{
    Quantity result(*this);
    result.m_numericValue /= other.m_numericValue;
    QMap<QString, Rational>::const_iterator i = other.m_dimension.constBegin();
    while (i != other.m_dimension.constEnd()) {
        const Rational & exp = i.value();
        const QString & name = i.key();
        if(!result.m_dimension.contains(name))
            result.m_dimension[name] = Rational(0);
        result.m_dimension[name] -= exp;
        ++i;
    }
    result.cleanDimension();
    return result;
}

Quantity Quantity::operator/(const HNumber & other) const
{
    return operator/(CNumber(other));
}

Quantity Quantity::operator/(const CNumber & other) const
{
    Quantity result(*this);
    result.m_numericValue /= other;
    result.cleanDimension();
    return result;
}

Quantity &Quantity::operator/=(const Quantity & other)
{
    return operator=(*this/other);
}

Quantity Quantity::operator%(const Quantity & other) const
{
    Quantity result(*this);
    result.m_numericValue = result.m_numericValue % other.m_numericValue;
    return result;
}

Quantity Quantity::operator&(const Quantity & other) const
{
    ENSURE_DIMENSIONLESS(*this);
    ENSURE_DIMENSIONLESS(other);
    Quantity result(*this);
    result.m_numericValue &= other.m_numericValue;
    return result;
}

Quantity &Quantity::operator&=(const Quantity &other)
{
    return operator=(*this & other);
}

Quantity Quantity::operator|(const Quantity &other) const
{
    ENSURE_DIMENSIONLESS(*this);
    ENSURE_DIMENSIONLESS(other);
    Quantity result(*this);
    result.m_numericValue |= other.m_numericValue;
    return result;
}

Quantity &Quantity::operator|=(const Quantity &other)
{
    return operator=(*this | other);
}

Quantity Quantity::operator^(const Quantity &other) const
{
    ENSURE_DIMENSIONLESS(*this);
    ENSURE_DIMENSIONLESS(other);
    Quantity result(*this);
    result.m_numericValue ^= other.m_numericValue;
    return result;
}

Quantity &Quantity::operator^=(const Quantity &other)
{
    return operator=(*this ^ other);
}

Quantity Quantity::operator~() const
{
    ENSURE_DIMENSIONLESS(*this);
    Quantity result(*this);
    result.m_numericValue= ~result.m_numericValue;
    return result;
}

Quantity Quantity::operator>>(const Quantity &other) const
{
    ENSURE_DIMENSIONLESS(*this);
    ENSURE_DIMENSIONLESS(other);
    Quantity result(*this);
    result.m_numericValue = result.m_numericValue >> other.m_numericValue;
    return result;
}

Quantity Quantity::operator<<(const Quantity &other) const
{
    ENSURE_DIMENSIONLESS(*this);
    ENSURE_DIMENSIONLESS(other);
    Quantity result(*this);
    result.m_numericValue = result.m_numericValue << other.m_numericValue;
    return result;
}




/* --------------------------------------------------------------------
 *    DMath
 * --------------------------------------------------------------------
 */


/*
 *  Wrappers for functions that are only defined for dimensionless arguments
 */

// no argument
#define COMPLEX_WRAPPER_DMATH_0(fct)        \
    Quantity DMath::fct()                   \
    {                                       \
        return Quantity(CMath::fct());      \
    }                                       \

// one argument
#define COMPLEX_WRAPPER_DMATH_1(fct)                            \
    Quantity DMath::fct(const Quantity &arg1)                   \
    {                                                           \
        ENSURE_DIMENSIONLESS(arg1);                             \
        return Quantity(CMath::fct(arg1.m_numericValue));       \
    }

// two arguments
#define COMPLEX_WRAPPER_DMATH_2(fct)                            \
    Quantity DMath::fct(const Quantity &arg1,                   \
                        const Quantity &arg2)                   \
    {                                                           \
        ENSURE_DIMENSIONLESS(arg1);                             \
        ENSURE_DIMENSIONLESS(arg2);                             \
        return Quantity(CMath::fct(arg1.m_numericValue,         \
                                   arg2.m_numericValue));       \
    }

// three arguments
#define COMPLEX_WRAPPER_DMATH_3(fct)                            \
    Quantity DMath::fct(const Quantity &arg1,                   \
                        const Quantity &arg2,                   \
                        const Quantity &arg3)                   \
    {                                                           \
        ENSURE_DIMENSIONLESS(arg1);                             \
        ENSURE_DIMENSIONLESS(arg2);                             \
        ENSURE_DIMENSIONLESS(arg3);                             \
        return Quantity(CMath::fct(arg1.m_numericValue,         \
                                   arg2.m_numericValue,         \
                                   arg3.m_numericValue));       \
    }

// four arguments
#define COMPLEX_WRAPPER_DMATH_4(fct)                            \
    Quantity DMath::fct(const Quantity &arg1,                   \
                        const Quantity &arg2,                   \
                        const Quantity &arg3,                   \
                        const Quantity &arg4)                   \
    {                                                           \
        ENSURE_DIMENSIONLESS(arg1);                             \
        ENSURE_DIMENSIONLESS(arg2);                             \
        ENSURE_DIMENSIONLESS(arg3);                             \
        ENSURE_DIMENSIONLESS(arg4);                             \
        return Quantity(CMath::fct(arg1.m_numericValue,         \
                                   arg2.m_numericValue,         \
                                   arg3.m_numericValue,         \
                                   arg4.m_numericValue));       \
    }


COMPLEX_WRAPPER_DMATH_0(e)
COMPLEX_WRAPPER_DMATH_0(pi)
COMPLEX_WRAPPER_DMATH_0(phi)
COMPLEX_WRAPPER_DMATH_0(i)

Quantity DMath::nan(Error error)
{
    return Quantity(CMath::nan(error));
}

COMPLEX_WRAPPER_DMATH_1(rad2deg)
COMPLEX_WRAPPER_DMATH_1(deg2rad)
COMPLEX_WRAPPER_DMATH_1(integer)
COMPLEX_WRAPPER_DMATH_1(frac)
COMPLEX_WRAPPER_DMATH_1(floor)
COMPLEX_WRAPPER_DMATH_1(ceil)
COMPLEX_WRAPPER_DMATH_1(exp)
COMPLEX_WRAPPER_DMATH_1(ln)
COMPLEX_WRAPPER_DMATH_1(lg)
COMPLEX_WRAPPER_DMATH_2(log)
COMPLEX_WRAPPER_DMATH_1(sinh)
COMPLEX_WRAPPER_DMATH_1(cosh)
COMPLEX_WRAPPER_DMATH_1(tanh)
COMPLEX_WRAPPER_DMATH_1(arsinh)
COMPLEX_WRAPPER_DMATH_1(arcosh)
COMPLEX_WRAPPER_DMATH_1(artanh)
COMPLEX_WRAPPER_DMATH_1(sin)
COMPLEX_WRAPPER_DMATH_1(cos)
COMPLEX_WRAPPER_DMATH_1(tan)
COMPLEX_WRAPPER_DMATH_1(sec)
COMPLEX_WRAPPER_DMATH_1(csc)
COMPLEX_WRAPPER_DMATH_1(arcsin)
COMPLEX_WRAPPER_DMATH_1(arccos)
COMPLEX_WRAPPER_DMATH_1(arctan)

COMPLEX_WRAPPER_DMATH_1(gamma)
COMPLEX_WRAPPER_DMATH_1(lnGamma)
COMPLEX_WRAPPER_DMATH_1(erf)
COMPLEX_WRAPPER_DMATH_1(erfc)

COMPLEX_WRAPPER_DMATH_2(gcd)
COMPLEX_WRAPPER_DMATH_2(idiv)

COMPLEX_WRAPPER_DMATH_2(nCr)
COMPLEX_WRAPPER_DMATH_2(nPr)
COMPLEX_WRAPPER_DMATH_3(binomialPmf)
COMPLEX_WRAPPER_DMATH_3(binomialCdf)
COMPLEX_WRAPPER_DMATH_2(binomialMean)
COMPLEX_WRAPPER_DMATH_2(binomialVariance)
COMPLEX_WRAPPER_DMATH_4(hypergeometricPmf)
COMPLEX_WRAPPER_DMATH_4(hypergeometricCdf)
COMPLEX_WRAPPER_DMATH_3(hypergeometricMean)
COMPLEX_WRAPPER_DMATH_3(hypergeometricVariance)
COMPLEX_WRAPPER_DMATH_2(poissonPmf)
COMPLEX_WRAPPER_DMATH_2(poissonCdf)
COMPLEX_WRAPPER_DMATH_1(poissonMean)
COMPLEX_WRAPPER_DMATH_1(poissonVariance)

COMPLEX_WRAPPER_DMATH_2(mask)
COMPLEX_WRAPPER_DMATH_2(ashr)

COMPLEX_WRAPPER_DMATH_3(decodeIeee754)
COMPLEX_WRAPPER_DMATH_4(decodeIeee754)
COMPLEX_WRAPPER_DMATH_3(encodeIeee754)
COMPLEX_WRAPPER_DMATH_4(encodeIeee754)


#if 0 // min and max are not defined in CMath
Quantity DMath::max(const Quantity &n1, const Quantity &n2)
{
    ENSURE_SAME_DIMENSION(n1, n2);
    Quantity result(n1);
    result.m_numericValue = CMath::max(n1.m_numericValue, n2.m_numericValue);
    return result;
}

Quantity DMath::min(const Quantity &n1, const Quantity &n2)
{
    ENSURE_SAME_DIMENSION(n1, n2);
    Quantity result(n1);
    result.m_numericValue = CMath::min(n1.m_numericValue, n2.m_numericValue);
    return result;
}
#endif

Quantity DMath::abs(const Quantity &n)
{
    Quantity result(n);
    result.m_numericValue = CMath::abs(n.m_numericValue);
    return result;
}

Quantity DMath::sqrt(const Quantity &n)
{
    Quantity result(n);
    result.m_numericValue = CMath::sqrt(n.m_numericValue);
    QMap<QString, Rational>::const_iterator i = result.m_dimension.constBegin();
    while (i != result.m_dimension.constEnd()) {
        const Rational & exp = i.value();
        const QString & name = i.key();
        result.modifyDimension(name, exp/2);
        ++i;
    }
    return result;
}

Quantity DMath::cbrt(const Quantity &n)
{
    Quantity result(n);
    result.m_numericValue = CMath::sqrt(n.m_numericValue);
    QMap<QString, Rational>::const_iterator i = result.m_dimension.constBegin();
    while (i != result.m_dimension.constEnd()) {
        const Rational & exp = i.value();
        const QString & name = i.key();
        result.modifyDimension(name, exp/3);
        ++i;
    }
    return result;
}

Quantity DMath::raise(const Quantity &n1, int n)
{
    Quantity result(n1);
    result.m_numericValue = CMath::sqrt(n1.m_numericValue);
    QMap<QString, Rational>::const_iterator i = result.m_dimension.constBegin();
    while (i != result.m_dimension.constEnd()) {
        const Rational & exp = i.value();
        const QString & name = i.key();
        result.modifyDimension(name, exp*n);
        ++i;
    }
    return result;
}

Quantity DMath::raise(const Quantity &n1, const Quantity &n2)
{
    if(!n2.isDimensionless()
            || (!n1.isDimensionless() && !n2.isReal()))
        return DMath::nan(InvalidDimension);


    // First get the new numeric value.
    Quantity result(CMath::raise(n1.m_numericValue, n2.m_numericValue));

    if(n1.isDimensionless())
        return result;

    // We can now assume that n1 has a dimension, but n2 is real.
    // Compute the new dimension: try to convert n2 to a Rational. If n2 is not
    // rational, return NaN.

    // For negative bases only allow odd denominators.
    Rational exponent(n2.m_numericValue.real);
    if(abs(exponent.toHNumber() - n2.m_numericValue.real) >= RATIONAL_TOL
       || (n1.isNegative() && exponent.denominator()%2 == 0))
        return HMath::nan(OutOfDomain);

    // Compute new dimension
    QMap<QString, Rational>::const_iterator i = n1.m_dimension.constBegin();
    while (i != n1.m_dimension.constEnd()) {
        result.modifyDimension(i.key(), i.value()*exponent);
        ++i;
    }

    return result;
}

Quantity DMath::sgn(const Quantity &x)
{
    return Quantity(CMath::sgn(x.m_numericValue));
}





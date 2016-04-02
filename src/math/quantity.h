// quantity.h
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

#ifndef QUANTITY_H
#define QUANTITY_H

#include <QMap>
#include "math/cmath.h"
#include "math/hmath.h"
#include "core/errors.h"

class CNumber;
class QString;
class Rational;
class QJsonObject;
class HNumber;

class DMath;
class Quantity;

Quantity operator*(const HNumber &l, const Quantity &r);
Quantity operator*(const CNumber &l, const Quantity &r);
Quantity operator/(const HNumber &l, const Quantity &r);
Quantity operator/(const CNumber &l, const Quantity &r);


class Quantity
{
    friend class DMath;
    friend Quantity operator-( const Quantity & );
    friend Quantity operator-( const Quantity &, const Quantity& );
    friend bool operator>( const Quantity& l, const Quantity& r );
    friend bool operator<( const Quantity& l, const Quantity& r );
    friend bool operator>=( const Quantity& l, const Quantity& r );
    friend bool operator<=( const Quantity& l, const Quantity& r );
    friend bool operator==( const Quantity& l, const Quantity& r );
    friend bool operator!=( const Quantity& l, const Quantity& r );

public:
    Quantity();
    Quantity(const Quantity& other);
    Quantity( int i );
    Quantity( const QJsonObject & json);
    Quantity(const HNumber &h);
    Quantity(const CNumber &c);
    ~Quantity();


    bool isNan() const;
    bool isZero() const;
    bool isReal() const;
    bool isPositive() const;
    bool isNegative() const;
    bool isInteger() const;

    bool hasUnit() const ;
    CNumber unit() const;
    QString unitName() const;
    CNumber numericValue() const;
    char format() const;
    Quantity& setDisplayUnit(const CNumber unit, const QString &name);
    Quantity& setFormat(char c);
    void stripUnits();
    bool hasDimension() const;
    bool isDimensionless() const;
    QMap<QString, Rational> getDimension() const;
    void modifyDimension(const QString & key, const Rational & exponent);
    void copyDimension(const Quantity & other);
    void clearDimension();
    bool sameDimension(const Quantity &other) const;
    void cleanDimension();

    void serialize(QJsonObject & json) const;
    static Quantity deSerialize(const QJsonObject & json);

    Error error() const;

    Quantity &operator=( const Quantity& other);
    Quantity operator+( const Quantity& ) const;
    Quantity &operator+=( const Quantity& );
    Quantity &operator-=( const Quantity& );
    Quantity operator*( const Quantity& ) const;
    Quantity operator*( const CNumber& ) const;
    Quantity operator*( const HNumber& ) const;
    Quantity &operator*=( const Quantity& );
    Quantity operator/( const Quantity& ) const;
    Quantity operator/( const HNumber& ) const;
    Quantity operator/( const CNumber& ) const;
    Quantity &operator/=( const Quantity& );
    Quantity operator%( const Quantity& ) const;
    Quantity operator&( const Quantity& ) const;
    Quantity &operator&=( const Quantity& );
    Quantity operator|( const Quantity& ) const;
    Quantity& operator|=( const Quantity& );
    Quantity operator^( const Quantity& ) const;
    Quantity& operator^=( const Quantity& );
    Quantity operator~() const;
    Quantity operator>>( const Quantity& ) const;
    Quantity operator<<( const Quantity& ) const;


private:
    CNumber m_numericValue;
    QMap<QString, Rational> m_dimension;
    CNumber * m_unit;
    QString m_unitName;
    char m_format;
};


/*
 * Math functions for quantities with dimensions
 */
class DMath
{
  public:
    static bool complexMode;

    static QString format(const Quantity q, char format=0, int prec=-1);

    static Quantity real(const Quantity& x);
    static Quantity imag(const Quantity& x);

    // CONSTANTS
    static Quantity e();
    static Quantity phi();
    static Quantity pi();
    static Quantity nan(Error error = Success);
    static Quantity i();
    // GENERAL MATH
    static Quantity rad2deg( const Quantity & angle );
    static Quantity deg2rad( const Quantity & angle );
    static Quantity abs( const Quantity & n );
    static Quantity integer( const Quantity & n );
    static Quantity frac( const Quantity & n );
    static Quantity floor( const Quantity & n );
    static Quantity ceil( const Quantity & n );
    static Quantity gcd( const Quantity & n1, const Quantity & n2 );
    static Quantity idiv( const Quantity& n1, const Quantity& n2 );
    static Quantity round( const Quantity & n, int prec = 0 );
    static Quantity trunc( const Quantity & n, int prec = 0 );
    static Quantity sqrt( const Quantity & n );
    static Quantity cbrt( const Quantity & n );
    static Quantity raise( const Quantity & n1, int n );
    static Quantity raise( const Quantity & n1, const Quantity & n2 );
    static Quantity sgn( const Quantity & x );
    // EXPONENTIAL FUNCTION AND RELATED
    static Quantity exp( const Quantity & x );
    static Quantity ln( const Quantity & x );
    static Quantity lg( const Quantity & x );
    static Quantity lb( const Quantity & x );
    static Quantity log( const Quantity & base, const Quantity & x );
    static Quantity sinh( const Quantity & x );
    static Quantity cosh( const Quantity & x );
    static Quantity tanh( const Quantity & x );
    static Quantity arsinh( const Quantity & x );
    static Quantity arcosh( const Quantity & x );
    static Quantity artanh( const Quantity & x );
    // TRIGONOMETRY
    static Quantity sin( const Quantity & x );
    static Quantity cos( const Quantity & x );
    static Quantity tan( const Quantity & x );
    static Quantity cot( const Quantity & x );
    static Quantity sec( const Quantity & x );
    static Quantity csc( const Quantity & x );
    static Quantity arcsin( const Quantity & x );
    static Quantity arccos( const Quantity & x );
    static Quantity arctan( const Quantity & x );
    // HIGHER MATH FUNCTIONS
    static Quantity factorial( const Quantity & x, const Quantity & base = CNumber(1) );
    static Quantity gamma( const Quantity & x);
    static Quantity lnGamma( const Quantity & x);
    static Quantity erf( const Quantity & x );
    static Quantity erfc( const Quantity & x );
    // PROBABILITY
    static Quantity nCr( const Quantity & n, const Quantity & k );
    static Quantity nPr( const Quantity & n, const Quantity & r );
    static Quantity binomialPmf( const Quantity & k, const Quantity & n, const Quantity & p );
    static Quantity binomialCdf( const Quantity & k, const Quantity & n, const Quantity & p );
    static Quantity binomialMean( const Quantity & n, const Quantity & p );
    static Quantity binomialVariance( const Quantity & n, const Quantity & p );
    static Quantity hypergeometricPmf( const Quantity & k, const Quantity & N, const Quantity & M, const Quantity & n );
    static Quantity hypergeometricCdf( const Quantity & k, const Quantity & N, const Quantity & M, const Quantity & n );
    static Quantity hypergeometricMean( const Quantity & N, const Quantity & M, const Quantity & n );
    static Quantity hypergeometricVariance( const Quantity & N, const Quantity & M, const Quantity & n );
    static Quantity poissonPmf( const Quantity & k, const Quantity & l );
    static Quantity poissonCdf( const Quantity & k, const Quantity & l );
    static Quantity poissonMean( const Quantity & l );
    static Quantity poissonVariance( const Quantity & l );
    // LOGIC
    static Quantity mask ( const Quantity & val, const Quantity & bits);
    static Quantity sgnext ( const Quantity & val, const Quantity & bits);
    static Quantity ashr( const Quantity & val, const Quantity & bits );
    // IEEE-754 CONVERSION
    static Quantity decodeIeee754( const Quantity & val, const Quantity & exp_bits,
                                  const Quantity & significand_bits );
    static Quantity decodeIeee754( const Quantity & val, const Quantity & exp_bits,
                                  const Quantity & significand_bits, const Quantity & exp_bias );
    static Quantity encodeIeee754( const Quantity & val, const Quantity & exp_bits,
                                  const Quantity & significand_bits );
    static Quantity encodeIeee754( const Quantity & val, const Quantity & exp_bits,
                                  const Quantity & significand_bits, const Quantity & exp_bias );
};

#endif // QUANTITY_H

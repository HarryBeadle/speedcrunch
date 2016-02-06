// cmath.h
// Complex number support : type definition and function for complex numbers.
//
// This file is part of the SpeedCrunch project
// Copyright (C) 2013, 2015-2016 Hadrien Theveneau <theveneau@gmail.com>.
// Copyright (C) 2015-2016 Pol Welter.
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

#ifndef MATH_CMATH_H
#define MATH_CMATH_H

#include "hmath.h"
#include "rational.h"

#include <QString>
#include <QMap>

class CMath;

class CNumber {
  friend class CMath;
  friend CNumber operator-( const CNumber & );
  friend CNumber operator-( const CNumber &, const CNumber& );
  friend bool operator>( const CNumber& l, const CNumber& r );
  friend bool operator<( const CNumber& l, const CNumber& r );
  friend bool operator>=( const CNumber& l, const CNumber& r );
  friend bool operator<=( const CNumber& l, const CNumber& r );
  friend bool operator==( const CNumber& l, const CNumber& r );
  friend bool operator!=( const CNumber& l, const CNumber& r );

  public:
    CNumber();
    CNumber( const HNumber& );
    CNumber( const HNumber&, const HNumber& );
    CNumber( const CNumber& );
    CNumber( int i );
    CNumber( const char* );
    CNumber( const QJsonObject & json );
    ~CNumber(){};

    bool isNan() const;
    bool isZero() const;
    bool isPositive() const;
    bool isNegative() const;
    bool isInteger() const;
    bool isGaussian() const;
    bool isReal() const;
    bool isNearReal() const;

//TODO formats should be separated from numbers

    // 'g': decimal general (default)
    // 'f': decimal fixed
    // 'e': decimal scientific
    // 'n': decimal engineering
    // 'h': hexadecimal
    // 'o': octal
    // 'b': binary
    char format() const;
    CNumber& setFormat( char c = 0 );

    /* The following functions assume that real and imaginary part have the same unit */
    /* so this point must be enforced by the functions who change the unit            */
    bool hasUnit() const { return real.hasUnit(); }
    CNumber getUnit() const { return real.getUnit(); }
    QString getUnitName() const { return real.getUnitName(); }
    CNumber& setDisplayUnit(const CNumber, const QString &name);
    void stripUnits();
    bool hasDimension() const { return real.hasDimension(); }
    bool isDimensionless() const { return real.isDimensionless(); }
    QMap<QString, Rational> getDimension() const { return real.getDimension(); }
    void modifyDimension(const QString & key, const Rational & exponent);
    void clearDimension();
    bool sameDimension(const CNumber & other) const { return real.sameDimension(other.real); }
    void cleanDimension();

    void serialize(QJsonObject & json) const;
    static CNumber deSerialize(const QJsonObject & json);

    int toInt() const;  // Removed because too much problematic for complex numbers
    Error error() const;

    CNumber& operator=( const CNumber& );
    CNumber operator+( const CNumber& ) const;
    CNumber& operator+=( const CNumber& );
    CNumber& operator-=( const CNumber& );
    CNumber operator*( const CNumber& ) const;
    CNumber operator*( const HNumber& ) const;
    CNumber operator*( int x) { return operator*( HNumber(x) ); } /* Overload ambiguity resolution */
    CNumber& operator*=( const CNumber& );
    CNumber operator/( const CNumber& ) const;
    CNumber operator/( const HNumber& ) const;
    CNumber operator/( int x) { return operator/( HNumber(x) ); } /* Overload ambiguity resolution */
    CNumber& operator/=( const CNumber& );
    CNumber operator%( const CNumber& ) const;
    CNumber operator&( const CNumber& ) const;
    CNumber& operator&=( const CNumber& );
    CNumber operator|( const CNumber& ) const;
    CNumber& operator|=( const CNumber& );
    CNumber operator^( const CNumber& ) const;
    CNumber& operator^=( const CNumber& );
    CNumber operator~() const;
    CNumber operator>>( const CNumber& ) const;
    CNumber operator<<( const CNumber& ) const;

  private:
    int compare( const CNumber & other ) const;

  public:
    HNumber real;
    HNumber imag;

    /* FIXME ! Better control of access to real and imag */

    /* Invariants :
       - real and imag have the same format settings
       - real and image are neither or both NaN
       - real and imag have the same NaN error       */
};

class CMath
{
  public:
    // FORMAT
    static char * format( const CNumber & n, char format = 'g', int prec = -1 );
    // CONSTANTS
    static CNumber e();
    static CNumber phi();
    static CNumber pi();
    static CNumber nan(Error error = Success);
    static CNumber i();
    // GENERAL MATH
    static CNumber rad2deg( const CNumber & angle );
    static CNumber deg2rad( const CNumber & angle );
    static CNumber max( const HNumber & n1, const HNumber & n2 );
    static CNumber min( const HNumber & n1, const HNumber & n2 );
    static CNumber abs( const CNumber & n );
    static CNumber integer( const CNumber & n );
    static CNumber frac( const CNumber & n );
    static CNumber floor( const CNumber & n );
    static CNumber ceil( const CNumber & n );
    static CNumber gcd( const CNumber & n1, const CNumber & n2 );
    static CNumber idiv( const CNumber& n1, const CNumber& n2 );
    static CNumber round( const CNumber & n, int prec = 0 );
    static CNumber trunc( const CNumber & n, int prec = 0 );
    static CNumber sqrt( const CNumber & n );
    static CNumber cbrt( const CNumber & n );
    static CNumber raise( const CNumber & n1, int n );
    static CNumber raise( const CNumber & n1, const CNumber & n2 );
    static CNumber sgn( const CNumber & x );
    // EXPONENTIAL FUNCTION AND RELATED
    static CNumber exp( const CNumber & x );
    static CNumber ln( const CNumber & x );
    static CNumber lg( const CNumber & x );
    static CNumber lb( const CNumber & x );
    static CNumber log( const CNumber & base, const CNumber & x );
    static CNumber sinh( const CNumber & x );
    static CNumber cosh( const CNumber & x );
    static CNumber tanh( const CNumber & x );
    static CNumber arsinh( const CNumber & x );
    static CNumber arcosh( const CNumber & x );
    static CNumber artanh( const CNumber & x );
    // TRIGONOMETRY
    static CNumber sin( const CNumber & x );
    static CNumber cos( const CNumber & x );
    static CNumber tan( const CNumber & x );
    static CNumber cot( const CNumber & x );
    static CNumber sec( const CNumber & x );
    static CNumber csc( const CNumber & x );
    static CNumber arcsin( const CNumber & x );
    static CNumber arccos( const CNumber & x );
    static CNumber arctan( const CNumber & x );
    // HIGHER MATH FUNCTIONS
    static CNumber factorial( const CNumber & x, const CNumber & base = CNumber(1) );
    static CNumber gamma( const CNumber & x);
    static CNumber lnGamma( const CNumber & x);
    static CNumber erf( const CNumber & x );
    static CNumber erfc( const CNumber & x );
    // PROBABILITY
    static CNumber nCr( const CNumber & n, const CNumber & k );
    static CNumber nPr( const CNumber & n, const CNumber & r );
    static CNumber binomialPmf( const CNumber & k, const CNumber & n, const CNumber & p );
    static CNumber binomialCdf( const CNumber & k, const CNumber & n, const CNumber & p );
    static CNumber binomialMean( const CNumber & n, const CNumber & p );
    static CNumber binomialVariance( const CNumber & n, const CNumber & p );
    static CNumber hypergeometricPmf( const CNumber & k, const CNumber & N, const CNumber & M, const CNumber & n );
    static CNumber hypergeometricCdf( const CNumber & k, const CNumber & N, const CNumber & M, const CNumber & n );
    static CNumber hypergeometricMean( const CNumber & N, const CNumber & M, const CNumber & n );
    static CNumber hypergeometricVariance( const CNumber & N, const CNumber & M, const CNumber & n );
    static CNumber poissonPmf( const CNumber & k, const CNumber & l );
    static CNumber poissonCdf( const CNumber & k, const CNumber & l );
    static CNumber poissonMean( const CNumber & l );
    static CNumber poissonVariance( const CNumber & l );
    // LOGIC
    static CNumber mask ( const CNumber & val, const CNumber & bits);
    static CNumber sgnext ( const CNumber & val, const CNumber & bits);
    static CNumber ashr( const CNumber & val, const CNumber & bits );
    // IEEE-754 CONVERSION
    static CNumber decodeIeee754( const CNumber & val, const CNumber & exp_bits,
                                  const CNumber & significand_bits );
    static CNumber decodeIeee754( const CNumber & val, const CNumber & exp_bits,
                                  const CNumber & significand_bits, const CNumber & exp_bias );
    static CNumber encodeIeee754( const CNumber & val, const CNumber & exp_bits,
                                  const CNumber & significand_bits );
    static CNumber encodeIeee754( const CNumber & val, const CNumber & exp_bits,
                                  const CNumber & significand_bits, const CNumber & exp_bias );
};

std::ostream & operator<<( std::ostream &, const CNumber & );

#endif // CMATH_CMATH_H

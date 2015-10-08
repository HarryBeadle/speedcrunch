/* cmath.cpp                                                               *
 *                                                                           *
 * by Hadrien Theveneau - 2013                                               *
 *                                                                           *
 * Complex number support : type definition and function for complex numbers */


#include <string.h>
#include <stdlib.h>
#include "floatconvert.h"
#include "cmath.h"
#include "cnumberparser.h"


/**
 * Creates a new complex number.
 */
CNumber::CNumber() :
  real(0),
  imag(0)
{}


/**
 * Creates a new complex number from one real number
 */
CNumber::CNumber( const HNumber& hn) :
  real(hn),
  imag(0)
{}


/**
 * Creates a new complex number from the real and imaginary parts
 */
CNumber::CNumber( const HNumber& x, const HNumber& y) :
  real(x),
  imag(y)
{}


/**
 * Copies from another complex number.
 */
CNumber::CNumber(const CNumber& cn) :
  real(cn.real),
  imag(cn.imag)
{}


/**
 * Creates a new number from an integer value.
 */
CNumber::CNumber(int i) :
  real(i),
  imag(0)
{}


///**
// * Creates a new number from a string.
// *
// * In this implemendation, only real number parsing is support.
// * Complex numbers are constructed by operations like 1+1*i.
// */
///* FIXME : Add support of (direct) complex number parsing */
//CNumber::CNumber(const char* str) :
//  real(str),
//  imag(0)
//{}


/**
 * Creates a new number from a string.
 */
CNumber::CNumber(const char* str) {
  CNumberParser parser(str);
  parser.parse(this);  /* FIXME ! Exception management ! */
}


/**
 * Returns true if this number is Not a Number (NaN).
 */
bool CNumber::isNan() const
{
  return real.isNan() || imag.isNan();
}


/**
 * Returns true if this number is zero.
 */
bool CNumber::isZero() const
{
  return real.isZero() && imag.isZero();
}


/**
 * Returns true if this number is a positive REAL
 */
bool CNumber::isPositive() const
{
  return real.isPositive() && imag.isZero();
}


/**
 * Returns true if this number is a negative REAL
 */
bool CNumber::isNegative() const
{
  return real.isNegative() && imag.isZero();
}


/**
 * Returns true if this number is integer and REAL
 */
bool CNumber::isInteger() const
{
  return real.isInteger() && imag.isZero();
}


/**
 * Returns true if this number is a Gaussian integer
 */
bool CNumber::isGaussian() const
{
  return real.isInteger() && imag.isInteger();
}


/**
 * Returns true if this number is a real number
 */
bool CNumber::isReal() const
{
  return imag.isZero();
}


/**
 * Returns the preferred format (base/precision), default is 0
 */
char CNumber::format() const
{
  /* real and imag have always the same format */
  return real.format();
}


/**
 * Sets the preferred format (base/precision), default is 0
 */
CNumber& CNumber::setFormat(char c)
{
  /* We must always ensure that both numbers have the same preferred format */
  real.setFormat(c);
  imag.setFormat(c);
  return *this;
}


/**
 * Returns a NaN (Not a Number) with error set to
 * passed parameter.
 */
CNumber CMath::nan(Error error)
{
  /* We must always ensure that both numbers have the same NaN error */
  CNumber result;
  result.real = HMath::nan(error);
  result.imag = HMath::nan(error);
  return result;
}


/**
 * Returns the error code kept with a NaN.
 */
Error CNumber::error() const
{
  /* real and imag have always the same NaN error */
  return real.error();
}


/**
 * Assigns from another complex number.
 */
CNumber& CNumber::operator=( const CNumber & cn )
{
  real = cn.real;
  imag = cn.imag;
  return *this;
}


/**
 * Adds another complex number.
 */
CNumber CNumber::operator+( const CNumber & num ) const
{
  CNumber result;
  result.real = real + num.real;
  result.imag = imag + num.imag;
  return result;
}


/**
 * Adds another complex number.
 */
CNumber& CNumber::operator+=( const CNumber & num )
{
  return operator=( *this + num );
}


/**
 * Subtract from another complex number.
 */
CNumber operator-( const CNumber & n1, const CNumber & n2 )
{
  CNumber result;
  result.real = n1.real - n2.real;
  result.imag = n1.imag - n2.imag;
  return result;
}


/**
 * Subtract from another complex number.
 */
CNumber& CNumber::operator-=( const CNumber & num )
{
  return operator=( *this - num );
}


/**
 * Multiplies with another complex number.
 */
CNumber CNumber::operator*( const CNumber & num ) const
{
  CNumber result;
  result.real = real*num.real - imag*num.imag;
  result.imag = imag*num.real + real*num.imag;
  return result;
}


/**
 * Multiplies with another number.
 */
CNumber& CNumber::operator*=( const CNumber & num )
{
  return operator=( *this * num );
}


/**
 * Divides with another complex number.
 */
CNumber CNumber::operator/( const CNumber & num ) const
{
  if (num.isZero())
    return CMath::nan(ZeroDivide);
  else {
    CNumber result;
    HNumber divider = num.real*num.real + num.imag*num.imag;
    result.real = (real*num.real + imag*num.imag) / divider;
    result.imag = (imag*num.real - real*num.imag) / divider;
    return result;
  }
}


/**
 * Divides with another REAL number.
 */
CNumber CNumber::operator/( const HNumber & num ) const
{
  if (num.isZero())
    return CMath::nan(ZeroDivide);
  else {
    return CNumber(real / num, imag / num)  ;
  }
}


/**
 * Divides with another number.
 */
CNumber& CNumber::operator/=( const CNumber & num )
{
  return operator=( *this / num );
}


/**
 * Modulo (rest of integer division)
 */
CNumber CNumber::operator%( const CNumber & num ) const
{
  if (isReal() && num.isReal())
    return *this % num;
  else
    /* FIXME ! Not supported on complex numbers */
    return CMath::nan(NotImplemented);
}


/**
 * Performs an integer divide
 */
CNumber CMath::idiv( const CNumber & dividend, const CNumber& divisor)
{
  if (dividend.isReal() && divisor.isReal())
    return HMath::idiv(dividend.real, divisor.real);
  else
    /* FIXME ! Not supported on complex numbers */
    return nan(NotImplemented);
}


/**
 * Returns -1, 0, 1 if n1 is less than, equal to, or more than n2.
 * Only valid for real numbers, since complex ones are not an ordered field.
 */
int CNumber::compare( const CNumber & other ) const
{
  if (isReal() && other.isReal())
    return real.compare(other.real);
  else
    /* Not supported on complex numbers */
    return false;      /* FIXME ! Return something better */
}

/**
 * Returns true if l is greater than r.
 */
bool operator>( const CNumber& l, const CNumber& r )
{
  return l.compare( r ) > 0;
}

/**
 * Returns true if l is less than r.
 */
bool operator<( const CNumber& l, const CNumber& r )
{
  return l.compare( r ) < 0;
}

/**
 * Returns true if l is greater than or equal to r.
 */
bool operator>=( const CNumber& l, const CNumber& r )
{
  return l.compare( r ) >= 0;
}

/**
 * Returns true if l is less than or equal to r.
 */
bool operator<=( const CNumber& l, const CNumber& r )
{
  return l.compare( r ) <= 0;
}

/**
 * Returns true if l is equal to r.
 */
bool operator==( const CNumber& l, const CNumber& r )
{
  return l.compare( r ) == 0;
}

/**
 * Returns true if l is not equal to r.
 */
bool operator!=( const CNumber& l, const CNumber& r )
{
  return l.compare( r ) != 0;
}


/**
 * Changes the sign.
 */
CNumber operator-( const CNumber & x )
{
  return CNumber(-x.real, -x.imag);
}


/**
 * Returns the constant e (Euler's number).
 */
CNumber CMath::e()
{
  return CNumber(HMath::e());
}


/**
 * Returns the constant pi.
 */
CNumber CMath::pi()
{
  return CNumber(HMath::pi());
}


/**
 * Returns the constant phi (golden number).
 */
CNumber CMath::phi()
{
  return CNumber(HMath::phi());
}


/**
 * Returns the constant i.
 */
CNumber CMath::i()
{
  return CNumber(0, 1);
}


/* TODO : Improve complex number formatting */

// /**
//  * Formats the given number as string, using specified decimal digits.
//  * Note that the returned string must be freed.
//  */
// char * CMath::formatFixed( const CNumber & cn, int prec )
// {
//   return format(cn, 'f', prec);
// }


// /**
//  * Formats the given number as string, in scientific format.
//  * Note that the returned string must be freed.
//  */
// char * CMath::formatScientific( const CNumber & cn, int prec )
// {
//   return format(cn, 'e', prec);
// }


/**
 * Formats the given number as string, in engineering notation.
 * Note that the returned string must be freed.
 */
char* CMath::format( const CNumber& cn, char format, int prec )
{
  /* If number is real */
  if (cn.isReal())

    /* Reverts to normal formatting */
    return HMath::format(cn.real, format, prec);

  /* If number is complex */
  else {

    /* Use complex number formatting */

    /* Format real part */
    char * real_part = HMath::format(cn.real, format, prec);

    /* Format imaginary part */
    const char * imag_part = "";
    const char * separator = "";
    const char * prefix    = "";   /* TODO : Insert two modes, one for a+jb and one for a+bj */
    const char * postfix   = "j";  /* TODO : Insert two modes, one for a+bi and one for a+bj */

    /* If imaginary part is positive */
    if (cn.imag.isPositive()) {
      separator = "+";
      imag_part = HMath::format(cn.imag,  format, prec);
    }
    /* If imaginary part is negative */
    else {
      separator = "-";
      imag_part = HMath::format(-cn.imag, format, prec);
    }

    /* Allocate string for the result */
    int l1 = strlen(real_part) + strlen(separator) + strlen(imag_part);
    char * result = (char *) malloc(l1);

    /* Concatenate parts of the result */
    strcpy(result, real_part);
    strcat(result, separator);
    strcat(result, prefix);
    strcat(result, imag_part);
    strcat(result, postfix);

    /* Free old strings */
    free((void *) real_part);
    free((void *) imag_part);

    return result;

  }
}


/**
 * Returns the norm of n.
 */
CNumber CMath::abs( const CNumber & n )
{
  return HMath::sqrt(n.real*n.real + n.imag*n.imag);
}


/**
 * Returns the square root of n.
 */
CNumber CMath::sqrt( const CNumber & n )
{
  CNumber result;

  /* s = 1 if imaginary part positive or null, -1 otherwise */
  HNumber s;
  if (n.imag.isPositive() || n.imag.isZero())
    s = 1;
  else
    s = -1;

  /* cf https://en.wikipedia.org/wiki/Square_root#Square_roots_of_negative_and_complex_numbers */
  result.real =     HMath::sqrt((abs(n).real + n.real) / 2);
  result.imag = s * HMath::sqrt((abs(n).real - n.real) / 2);

  return result;
}


/**
 * Raises n1 to an integer n.
 */
CNumber CMath::raise( const CNumber & n1, int n )
{
  return CMath::exp(CMath::ln(n1) * n);
}


/**
 * Raises n1 to n2.
 */
CNumber CMath::raise( const CNumber & n1, const CNumber & n2  )
{
  return CMath::exp(CMath::ln(n1) * n2);
}


/**
 * Returns e raised to x.
 */
CNumber CMath::exp( const CNumber & x )
{
  HNumber abs = HMath::exp(x.real);

  return CNumber(abs*HMath::cos(x.imag), abs*HMath::sin(x.imag));
}


/**
 * Returns the complex natural logarithm of x.
 */
CNumber CMath::ln( const CNumber & x )
{
  HNumber abs = CMath::abs(x).real;

  CNumber result;
  result.real = HMath::ln(abs);

  if (x.imag.isPositive())
    result.imag =  HMath::arccos(x.real / abs);
  else
    result.imag = -HMath::arccos(x.real / abs);

  return result;
}


/**
 * Returns the common logarithm of x.
 */
CNumber CMath::lg( const CNumber & x )
{
  return CMath::ln(x) / HMath::ln(10);
}


/**
 * Returns the binary logarithm of x.
 */
CNumber CMath::lb( const CNumber & x )
{
  return CMath::ln(x) / HMath::ln(2);
}


/**
 * Returns the logarithm of x to base.
 * If x is non positive, returns NaN.
 */
CNumber CMath::log( const CNumber & base, const CNumber & x )
{
  return CMath::ln(x) / CMath::ln(base);
}


/**
 * Returns the complex sine of x. Note that x must be in radians.
 */
CNumber CMath::sin( const CNumber & x )
{
  /* cf. https://en.wikipedia.org/wiki/Sine#Sine_with_a_complex_argument */
  return CNumber(HMath::sin(x.real)*HMath::cosh(x.imag), HMath::cos(x.real)*HMath::sinh(x.imag));
}


/**
 * Returns the cosine of x. Note that x must be in radians.
 */
CNumber CMath::cos( const CNumber & x )
{
  /* Expanded using Wolfram Mathematica 9.0 */
  return CNumber(HMath::cos(x.real)*HMath::cosh(x.imag), -HMath::sin(x.real)*HMath::sinh(x.imag));
}


/**
 * Returns the tangent of x. Note that x must be in radians.
 */
CNumber CMath::tan( const CNumber & x )
{
  return CMath::sin(x) / CMath::cos(x);
}


/**
 * Returns the hyperbolic sine of x.
 */
CNumber CMath::sinh( const CNumber & x )
{
  return (exp(x) - exp(-x)) / HNumber(2);
}


/**
 * Returns the hyperbolic cosine of x.
 */
CNumber CMath::cosh( const CNumber & x )
{
  return (exp(x) + exp(-x)) / HNumber(2);
}


/**
 * Returns the hyperbolic tangent of x.
 */
CNumber CMath::tanh( const CNumber & x )
{
  return sinh(x) / cosh(x);
}


/*********************************************************/
/* DUMMY FUNCTIONS, JUST FOR TEST                        */
/* WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!              */
/*********************************************************/

#define NOT_IMPLEMENTED_CNUMBER_1(fct) \
  CNumber CNumber::fct( ) const { \
    return CMath::nan(NotImplemented); \
  }

#define NOT_IMPLEMENTED_CNUMBER_2(fct) \
  CNumber CNumber::fct( const CNumber & ) const { \
    return CMath::nan(NotImplemented); \
  }

#define NOT_IMPLEMENTED_CNUMBER_3(fct) \
  CNumber& CNumber::fct( const CNumber & ) { \
    *this = CMath::nan(NotImplemented); \
    return *this; \
  }

#define NOT_IMPLEMENTED_CNUMBER_4(fct) \
  int CNumber::fct() const { \
    return 0; \
  }

#define NOT_IMPLEMENTED_CMATH_NUM(fct) \
  CNumber CMath::fct( const CNumber & ) { \
    return CMath::nan(NotImplemented); \
  }

#define NOT_IMPLEMENTED_CMATH_NUM_NUM(fct) \
  CNumber CMath::fct( const CNumber &, const CNumber &) { \
    return CMath::nan(NotImplemented); \
  }

#define NOT_IMPLEMENTED_CMATH_NUM_INT(fct) \
  CNumber CMath::fct( const CNumber &, int ) { \
    return CMath::nan(NotImplemented); \
  }

#define NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM(fct) \
  CNumber CMath::fct( const CNumber &, const CNumber &, const CNumber & ) {	\
    return CMath::nan(NotImplemented); \
  }

#define NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM_NUM(fct) \
  CNumber CMath::fct( const CNumber &, const CNumber &, const CNumber &, const CNumber & ) {	\
    return CMath::nan(NotImplemented); \
  }

// CNumber
NOT_IMPLEMENTED_CNUMBER_4( toInt );
NOT_IMPLEMENTED_CNUMBER_2( operator& );
NOT_IMPLEMENTED_CNUMBER_3( operator&= );
NOT_IMPLEMENTED_CNUMBER_2( operator| );
NOT_IMPLEMENTED_CNUMBER_3( operator|= );
NOT_IMPLEMENTED_CNUMBER_2( operator^ );
NOT_IMPLEMENTED_CNUMBER_3( operator^= );
NOT_IMPLEMENTED_CNUMBER_1( operator~ );
NOT_IMPLEMENTED_CNUMBER_2( operator>> );
NOT_IMPLEMENTED_CNUMBER_2( operator<< );
// CMath GENERAL MATH
NOT_IMPLEMENTED_CMATH_NUM( rad2deg );
NOT_IMPLEMENTED_CMATH_NUM( deg2rad );
NOT_IMPLEMENTED_CMATH_NUM( integer );
NOT_IMPLEMENTED_CMATH_NUM( frac );
NOT_IMPLEMENTED_CMATH_NUM( floor );
NOT_IMPLEMENTED_CMATH_NUM( ceil );
NOT_IMPLEMENTED_CMATH_NUM_NUM( gcd );
NOT_IMPLEMENTED_CMATH_NUM_INT( round );
NOT_IMPLEMENTED_CMATH_NUM_INT( trunc );
NOT_IMPLEMENTED_CMATH_NUM( cbrt );
NOT_IMPLEMENTED_CMATH_NUM( sgn );
// CMath EXPONENTIAL FUNCTION AND RELATED
NOT_IMPLEMENTED_CMATH_NUM( arcsin );
NOT_IMPLEMENTED_CMATH_NUM( arccos );
NOT_IMPLEMENTED_CMATH_NUM( arctan );
NOT_IMPLEMENTED_CMATH_NUM( arsinh );
NOT_IMPLEMENTED_CMATH_NUM( arcosh );
NOT_IMPLEMENTED_CMATH_NUM( artanh );
// CMath TRIGONOMETRY
NOT_IMPLEMENTED_CMATH_NUM( cot );
NOT_IMPLEMENTED_CMATH_NUM( sec );
NOT_IMPLEMENTED_CMATH_NUM( csc );
// CMath HIGHER MATH FUNCTIONS
NOT_IMPLEMENTED_CMATH_NUM_NUM( factorial );
NOT_IMPLEMENTED_CMATH_NUM( erf );
NOT_IMPLEMENTED_CMATH_NUM( erfc );
NOT_IMPLEMENTED_CMATH_NUM( gamma );
NOT_IMPLEMENTED_CMATH_NUM( lnGamma );
// CMath PROBABILITY
NOT_IMPLEMENTED_CMATH_NUM_NUM( nCr );
NOT_IMPLEMENTED_CMATH_NUM_NUM( nPr );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM( binomialPmf );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM( binomialCdf );
NOT_IMPLEMENTED_CMATH_NUM_NUM( binomialMean );
NOT_IMPLEMENTED_CMATH_NUM_NUM( binomialVariance );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM_NUM( hypergeometricPmf );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM_NUM( hypergeometricCdf );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM( hypergeometricMean );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM( hypergeometricVariance );
NOT_IMPLEMENTED_CMATH_NUM_NUM( poissonPmf );
NOT_IMPLEMENTED_CMATH_NUM_NUM( poissonCdf );
NOT_IMPLEMENTED_CMATH_NUM( poissonMean );
NOT_IMPLEMENTED_CMATH_NUM( poissonVariance );
// CMath LOGIC
NOT_IMPLEMENTED_CMATH_NUM_NUM( mask );
NOT_IMPLEMENTED_CMATH_NUM_NUM( sgnext );
NOT_IMPLEMENTED_CMATH_NUM_NUM( ashr );
// CMath IEEE-754 CONVERSION
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM( decodeIeee754 );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM_NUM( decodeIeee754 );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM( encodeIeee754 );
NOT_IMPLEMENTED_CMATH_NUM_NUM_NUM_NUM( encodeIeee754 );


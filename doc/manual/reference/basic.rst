Basic math functions
====================

General
-------

.. function:: abs(x)

    Returns the absolute value of ``x``, commonly written as \|x\|. In real mode, it flips the sign of negative number, thus enforcing it to be positive. When given a complex number as argument, it returns the modulus of the number.

    The argument can have a dimension.

    .. admonition:: Example

        .. code-block:: python

            abs(-3 meter)     = 3 meter
            abs(4 + 3j)       = 5
  
.. function:: sqrt(x)
    
    Returns the square root of *x*. Without **complex numbers** are enabled, :func:`sqrt` is only defined for *x > 0*. In complex mode, any complex number may be supplied to :func:`sqrt`, yielding the complex root in the upper half plane.
    
    The argument may have a dimension.

.. function:: cbrt(x)

    Computes the third (cubic) root of *x*.
    
    In **real mode**, it accepts any real number. Negative numbers will yield a negative cubic root. ::
    
        cbrt(-27) = -3
    
    In **complex mode**, :func:`cbrt` accetps any complex input. The result will generally be the first complex root, i.e. the one with a phase between 0 and π/3. Real negative arguments will however still yield a real (negative) result, thus complying with the function's behaviour in **real mode**. Use ``x^(1/3)`` to enforce the first complex root.

.. function:: exp(x)

    Computes the natural exponential function.
    
    The argument must be dimensionless.
    
    .. seealso ::
        * :func:`ln` (natural logarithm)
        * :func:`lg` (binary logarithm)
        * :func:`log` (arbitrary base logarithm)
        
.. function:: ln(x)
    
    Computes the natural logarithm. In **real mode**, the argument must be real and >0.
    
    In **complex mode** any non-zero number may be supplied. The result will be the principal value. The branch cut runs across the negative real axis. Nevertheless, in SpeedCrunch :func:`ln` is defined for negative real numbers, giving `ln(-x) = ln(\|x\|)) + πj``, extending the branch from the *upper* half-plane.

.. function:: lb(x)

    Computes the binary logarithm. In **complex mode** the same rules apply as for :func:`ln`.
    
.. function:: lg(x)

    Computes the decimal logarithm. In **complex mode** the same rules apply as for :func:`ln`.
    
.. function:: log(n; x)

    Computes the logarithm of base *n* . In **complex mode** the same rules apply as for :func:`ln`.
     

Trigonometric & inverse trigonometric
-------------------------------------

.. function:: sin(x)

    Returns the sine of *x*. The behaviour depends on both **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.
    
    In **degrees** mode, the argument is ssumed to be expressed in degrees, such that :func:`sin` is periodic with a period of 360 degrees: *sin(x) = sin(x+360)*. Complex arguments are not allowed in **degrees** mode, regardless of the corresponding setting.
    
    When **radians** are set as angle unit, :func:`sin` will be 2π-periodic. The argument may be complex.
    
    For real argumenents beyond approx. \|x\|>10\ :sup:`77`, SpeedCrunch no longer recognizes the periodicity of the function and issues an error.
    
    The argument of :func:`sin` must be dimensionless.
    
    The inverse function is :func:`arcsin`.
    
    .. seealso ::
        * :func:`cos`
        * :func:`tan`
        * :func:`cot`
    
.. function:: cos(x)

    Returns the cosine of *x*. The behaviour depends on both **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.
    
    In **degrees** mode, the argument is ssumed to be expressed in degrees, such that :func:`cos` is periodic with a period of 360 degrees: *cos(x) = cos(x+360)*. Complex arguments are not allowed in **degrees** mode, regardless of the corresponding setting.
    
    When **radians** are set as angle unit, :func:`cos` will be 2π-periodic. The argument may be complex.
    
    For real argumenents beyond approx. \|x\|>10\ :sup:`77`, SpeedCrunch no longer recognizes the periodicity of the function and issues an error.
    
    The argument of :func:`cos` must be dimensionless.
    
    The inverse function is :func:`arccos`.
    
    .. seealso ::
        * :func:`sin`
        * :func:`tan`
        * :func:`cot`
        * :func:`sec`
        * :func:`arccos`

.. function:: tan(x)

    Returns the tangent of *x*. The behaviour depends on both **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.
    
    In **degrees** mode, the argument is ssumed to be expressed in degrees, such that :func:`tan` is periodic with a period of 180 degrees: *tan(x) = tan(x+180)*. Complex arguments are not allowed in **degrees** mode, regardless of the corresponding setting.
    
    When **radians** are set as angle unit, :func:`tan` will be π-periodic. The argument may be complex.
    
    The argument of :func:`tan` must be dimensionless.
    
    The inverse function is :func:`arctan`.
    
    .. seealso ::
        * :func:`cos`
        * :func:`sin`
        * :func:`cot`
        
.. function:: cot(x)

    Returns the cotangent of *x*. The behaviour depends on both **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.
    
    In **degrees** mode, the argument is ssumed to be expressed in degrees, such that :func:`cot` is periodic with a period of 180 degrees: *cot(x) = cot(x+180)*. Complex arguments are not allowed in **degrees** mode, regardless of the corresponding setting.
    
    When **radians** are set as angle unit, :func:`cot` will be π-periodic. The argument may be complex.
    
    The argument of :func:`cot` must be dimensionless.
    
    .. seealso ::
        * :func:`cos`
        * :func:`sin`
        * :func:`tan`
        
.. function:: sec(x)

    Returns the secant of *x*, defined as the reciprocal cosine of *x*: `sec(x) = 1/cos(x)` . The behaviour depends on both **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.
    
    In **degrees** mode, the argument is ssumed to be expressed in degrees, such that :func:`sec` is periodic with a period of 360 degrees: *sec(x) = sec(x+360)*. Complex arguments are not allowed in **degrees** mode, regardless of the corresponding setting.
    
    When **radians** are set as angle unit, :func:`sec` will be 2π-periodic. The argument may be complex.
    
    For real argumenents beyond approx. \|x\|>10\ :sup:`77`, SpeedCrunch no longer recognizes the periodicity of the function and issues an error.
    
    The argument of :func:`cos` must be dimensionless.
    
.. function:: csc(x)
    
    Returns the cosecant of *x*.
    
    .. warning::
        TODO: Add more info


.. function:: arccos(x)

    Returns the inverse cosine of *x*, such that ``cos(arccos(x)) = x``.

    The behaviour of the function depends on both the **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.

    In **degreees** mode, :func:`arccos` takes a real argument from [--1, 1], and the return value is in the range [0, 180]. Real arguments outside [--1, 1] and complex numbers are not allowed in **degrees** mode.

    When **radians** are set as angle unit, :func:`arccos` maps an element from [--1, 1] to a value in [0, π]. When **complex numbers** are enabled in addition, :func:`arccos` may take any argument from the complex plane. In complex mode, ``arccos(-1) = π`` and ``arccos(1) = 0`` will yield the same result as in real mode.

    The argument of :func:`arccos` must be dimensionless.
    
    The inverse function is :func:`cos`.

.. function:: arcsin(x)

    Returns the inverse sine of *x*, such that ``sin(arcsin(x)) = x``.

    The behaviour of the function depends on both the **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.

    In **degreees** mode, :func:`arcsin` takes a real argument from [--1, 1], and the return value is in the range [--90, 90]. Real arguments outside [--1, 1] and complex numbers are not allowed in **degrees** mode.

    When **radians** are set as angle unit, :func:`arcsin` maps an element from [--1, 1] to a value in [--π/2, π/2]. When **complex numbers** are enabled in addition, :func:`arcsin` may take any argument from the complex plane. In complex mode, ``arcsin(-1) = π/2`` and ``arcsin(1) = π/2`` will yield the same result as in real mode.

    The argument of :func:`arccos` must be dimensionless.
    
    The inverse function is :func:`sin`.
    
 
.. function:: arctan(x)

    Returns the inverse sine of *x*, such that ``tan(arctan(x)) = x``.

    The behaviour of the function depends on both the **angle mode** (degrees or radians) and on whether **complex numbers** are enabled.

    In **degreees** mode, :func:`arctan` takes a real argument, and the return value is in the range [-90, 90]. Complex arguments are not allowed in **degrees** mode.

    When **radians** are set as angle unit, :func:`arctan` maps a real number to a value in [-π/2, π/2]. When **complex numbers** are enabled in addition, :func:`arctan` may take any argument from the complex plane, except for +/- j.

    The argument of :func:`arctan` must be dimensionless.
    
    The inverse function is :func:`tan`.
    

Hyperbolic & inverse hyperbolic
-------------------------------

.. function:: sinh(x)

    Returns the hyperbolic sine of *x*. In **complex mode** any complex number may be used as argument.
    
    The argument must be dimensionless.
    
    The inverse function is :func:`arsinh`.
    
.. function:: cosh(x)

    Returns the hyperbolic cosine of *x*. In **complex mode** any complex number may be used as argument.
    
    The argument must be dimensionless.
    
    The inverse function is :func:`arcosh`.
    
.. function:: tanh(x)

    Returns the hyperbolic tangent of *x*. In **complex mode** any complex number may be used as argument.
    
    The argument must be dimensionless.
    
    The inverse function is :func:`artanh`.
    
.. function:: arsinh(x)

    Computes the area hyperbolic sine of `x`, the inverse fuction to :func:`sinh`. `arsinh(x)` is the only solution to *cosh(y) = x*.

    In complex mode, the function is defined for any complex *z* by arsinh(z) = ln[z + (z\ :sup:`2`+1)\ :sup`1/2`].

    The function only accepts dimensionless arguments. 
    
.. function:: arcosh(x)

    Computes the area hyperbolic cosine of `x`, the inverse fuction to :func:`cosh`. `arcosh (x)` is the positive solution to *cosh(y) = x*. Except for `x=1`, the second solution to this equatiopn will be given by `--arcosh(x)`.

    In real mode, the parameter `x` must be >1. In complex mode, the function is defined for any complex *z* by arcosh(z) = ln[z + (z\ :sup:`2`-1)\ :sup:`2`].

    The function only accepts dimensionless arguments. 

    
.. function:: artanh(x)

    Computes the area hyperbolic tangent of `x`, the inverse function to tanh y. `artanh (x)` is the only solution to tanh(y) = x*. In real mode, the parameter `x` has to fulfil -1 < `x` < 1.

    In complex mode, `artanh` takes any argument, except for -1 and +1. In the complex plane it is defined by artanh(z) = 1/2 * ln[(z+1)/(z-1)].

    The function only accepts dimensionless arguments.

Special
-------
.. function:: erf(x)
              erfc(x)
              gamma(x)
              lngamma(x)
              

Various
-------

.. function:: sgn(x)
              radians(x)
              degrees(x)
              frac(x)
              int(x)
    
 
Integer & bitwise operations
============================

Note that unlike in most programming languages, SpeedCrunch does *not* represent negative binary numbers by their two's complement. This is due to the fact that mathengine does not represent an integer with a fixed number of bits. Instead, the sign is stored as a separate property. Thus, while usually the signed 16-bit integers *0xFFFF* and *-1* are identical, they are distinct in SpeedCrunch.

To specify large integers, prefer the use of the shift operators (``1 << n``) to exponentiation (``2^n``). The latter will be performed as floating point, and will be prone to rounding errors. In particular, the result might not be integer.

.. function:: and(x1; x2; ...)

   Performs a bitwise logical AND on the submitted parameters (one or more). All parameters  have to be real integers from the range --2\ :sup:`255` to +2\ :sup:`255`--1 (signed or unsigned 256 bit integers), non integer arguments are rounded towards zero. The result ranges from --2\ :sup:`255` to +2\ :sup:`255`--1 (signed integer). Note that ``and(x)`` is not the identity, because the unsigned +2\ :sup:`255` is mapped to the signed --2\ :sup:`255` for example. An error is returned if the parameters are not in the valid range.
    
.. function:: or(x1; x2; ...)

   Performs a bitwise logical OR on the submitted parameters (one or more). All parameters have to be integers from the range --2\ :sup:`255` to +2\ :sup:`255`--1 (signed integer), non integer arguments are rounded towards zero. Note that ``or(x)`` is not the identity, because the unsigned 2\ :sup:`255` is mapped to the signed --2\ :sup:`255`, for example.
    
.. function:: xor(x1; x2; ...)

   Performs a bitwise logical XOR on the submitted parameters (one or more). All parameters have to be integers from the range --2\ :sup:`255` to +2\ :sup:`255`--1  (signed integer), non integer arguments are rounded towards zero. Note that ``xor(x)`` is not the identity, because the unsigned 2\ :sup:`255` is mapped to the signed --2\ :sup:`255`, for example.

.. function:: not(n)

   The :func:`not` function is defined by ``not(x) = -x-1``, giving the same result as the one's complement operator ``~`` in C/C++.
    
    .. warning:: The function does *not* simpy flip the bits!
    
    
.. function::   bin(n)
                dec(n)
                oct(n)
                hex(n)
                ceil(x)
                floor(x)
                idiv(dividend; divisor)
                mask(n; bits)
                unmask(n; bits)
                round(x; digits)
                shr(n; bits)
                shl(n; bits)
                trunc(x; digits)
                gcd()
                mod()
                
                

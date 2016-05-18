Syntax
======

This part of the documentation explains the syntax of valid SpeedCrunch input. As you will see, SpeedCrunch honors most conventions for mathematical expressions. You will find using SpeedCrunch to be very natural and intuitive, especially so if you are already familiar with a programming language.


Numbers
-------

Decimal Form
++++++++++++

When you would like to specify a non-integer value, simply enter the number as you would write it on paper,
with either a period (``.``) or a comma (``,``) as the decimal separator. By default, these can be
used interchangeably, i.e. both ``1.234`` and ``1,234`` represent the same number. However, this
behavior can be changed; see :ref:`Radix Character <radix_character>` for more information.

This also implies that neither comma nor period can be used for digit grouping. For instance, ``1,234.567`` is not a valid input. Instead, use spaces or apostrophes (``'``) to group digits, e.g. ``1 000 000`` or ``1'000'000`` for a million.

Trailing zeros after the decimal point (like in ``12.300``) or leading zeros before it (``0012.3``) are redundant and can be included or omitted to the user's preference. Expressions like ``.5`` as a shorthand notation for ``0.5`` are also permitted.


.. _scientific_notation:

Scientific Notation
+++++++++++++++++++

When dealing with very small or very large numbers (think size of an atom, or a galaxy) the notation above is inconvenient. These are more commonly expressed in scientific notation; for instance, 1.234*10\ :sup:`-9` is preferable to 0.000000001234.

Naturally, in SpeedCrunch this could be written as ``1.234*10^-9``, but there's also a shorthand notation: ``1.234e-9``. Here, the ``e`` represents ``*10^``, but it is considered a part of the number literal and treated with higher precedence. For example, ``1e2^3`` is equivalent to ``(1e2)^3 = 100^3``. The scale of a number (sometimes called its exponent) always begins with the scale character ``E`` or ``e`` followed by a signed integer. So ``e+10``, ``e-4``, ``E-0`` are all valid scale expressions. If the sign is '+', you may simply omit it: ``e0``, ``E10``. The significand (i.e. the part preceding the exponent) is required; exactly one exponent must be specified.

Compared to most calculators, SpeedCrunch can accept very large numbers without overflowing (e.g. both ``1e+536870911`` and ``1e-536870911`` are still valid). However, only about 78 significant digits are stored at any point. Any digits beyond that are lost.

Non-Decimal Bases
+++++++++++++++++

Sometimes you may require to work in numeral bases other than decimal. SpeedCrunch provides support for binary (base-2), octal (base-8) and hexadecimal (base-16) numbers.

You can enter a number in any of these bases by marking it with the corresponding prefix:

* ``0b`` or ``0B`` for **binary**, e.g. ``0b10010``.
* ``0o`` or ``0O`` for **octal**, e.g. ``0o1412``.
* ``0d`` or ``0D`` for **decimal**. These can be omitted since decimal is the default base.
* ``0x``, ``0X``, ``0h``, ``0H`` or ``#`` for **hexadecimal**. The additional six digits are represented by the upper or lower case letters ``a`` to ``f``, e.g. ``0xdeadbeef`` or ``0xDEADBEEF``.

You may even enter fractional values in any of these bases. Note that scientific notation is not
supported for non-decimal bases, however. Examples::

    0b1.01
    = 1.25

    0xf.a
    = 15.625

To have SpeedCrunch output its results in a base other than decimal, you may use one of the functions :func:`bin`, :func:`oct`, :func:`dec`, or :func:`hex`::

    hex(12341)
    = 0x3035

The effect of these functions only applies to the immediate result and doesn't carry to future
operations::

    0x2 * hex(12341)
    = 24682

To change the base that is used for displaying results, select one of the corresponding settings in :menuselection:`Settings --> Result Format`.

SpeedCrunch stores integers with a precision of up to 256 bits. Internally, negative numbers are
not stored in the two's complement form that's commonly used for negative integers in computing;
instead, the absolute value and the sign are stored separately. For this reason, the binary
representation of a negative number in SpeedCrunch is *not* its two's complement. See :func:`mask`
and :func:`unmask` to convert a negative number into the two's complement form.


Operators and Precedence
------------------------

When writing an expression like ``10+5*4``, which operation will be executed first? The common rules of operator precedence tell us that in this case multipication shall be computed first, hence the result is ``30``. We also distinguish **unary** operators (which act on a single number/operand) and **binary** operators (which link two operands).

SpeedCrunch supports the following operators, listed in decreasing order of precedence:

* Parentheses ``(...)``. Use these to mark precedence explicitly, e.g. ``(2+3)*4 = 5*4 = 20``.
* Unary operator ``!``. Computes the factorial of its argument, e.g. ``5! = 125``; see also :func:`gamma`.
* Binary operator ``^`` or ``**`` (power). Both notations (``^`` and ``**``) are equivalent. Note that the power operation is *right-associative*, i.e. evaluated from right to left, e.g. ``2^2^3 = 2^8 = 256``.
* Simplified function syntax, e.g. ``sqrt 2``. [#simplified_function]_.
* Binary operator ``\`` (integer division).
* Binary operators ``*`` (multiplication) and ``/`` (division), unary operators ``+`` and ``-`` (negation), implied multiplication [#implied_mult]_, and unary operator ``%`` (percent) [#percent]_.
* Binary operators ``+`` (addition) and ``-`` (subtraction).
* Binary operators ``<<`` (left shift) and ``>>`` (right shift); see also :func:`shl` and :func:`shr`.
* Binary operator ``&`` (bitwise and); see also :func:`and`.
* Binary operator ``|`` (bitwise or); see also :func:`or`.
* Binary operator ``->`` or ``in`` (unit conversion). Again, both notations are equivalent. See :ref:`units` for more information.


.. rubric:: Footnotes

.. [#simplified_function]
    .. deprecated :: 0.12
        *Simplified function syntax* refers to the notation where the parentheses are omitted from a function call, e.g. ``sin 123``. The use of this feature is **discouraged**, as it easily allows for errors to creep in. It is deprecated as of version 0.12 and may be removed in a later version of SpeedCrunch.

.. [#implied_mult]
    .. versionadded :: 0.12
        *Implicit multiplication* allows multiplication without the multiplication operator (``*``), for instance: ``3 sqrt(2)``.

.. [#percent]
    .. deprecated :: 0.12
        The percent operator ``%`` has been removed: it was confusing and not very useful. The reasons for its removal are discussed in more detail in `issue #239 <issue239_>`_. In versions prior to 0.12, the percent operator was equivalent to multiplication by 0.01, e.g. ``10% = 0.1``, or ``5 - 20% = 4.8``.

.. _issue239: https://bitbucket.org/heldercorreia/speedcrunch/issues/239/more-intuitive-and-useful-percentage

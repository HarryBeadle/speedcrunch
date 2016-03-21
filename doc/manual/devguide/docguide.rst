Documentation Guide
===================

The SpeedCrunch manual -- the one you're reading right now -- is created using the
`Sphinx documentation generator <sphinx_>`_.
The aim of this document is two-fold: to give an introduction to Sphinx and reStructuredText --
mostly by referring to the Sphinx documentation -- and to document any additional
markup directives, guidelines, and commands for writing the SpeedCrunch documentation.

.. _sphinx: http://sphinx-doc.org



Sphinx Intro
------------

Sphinx documentation is written in reStructuredText (reST), a lightweight plain-text markup format,
and then transformed into various output formats like HTML. If you want to learn more
about using Sphinx in general, your best bet is unsurprisingly to start with the `Sphinx tutorial <sphinx-tut_>`_
and then read on from there.

.. _sphinx-tut: http://sphinx-doc.org/en/stable/tutorial.html

However, if you're mainly interested in contributing to existing documentation, your first
stop should be the `reStructuredText Primer <rst-primer_>`_ which explains the basics
of reST. In addition, you'll want to read the section on `additional Sphinx markup constructs <sphinx-markup_>`_
to get an overview of any other semantic markup elements Sphinx offers.

.. _rst-primer: http://sphinx-doc.org/en/stable/rest.html
.. _sphinx-markup: http://sphinx-doc.org/en/stable/markup/index.html



Style Guide
-----------

* When using external links with a separate target, you should always specify an
  explicit identifier for the link. That means you **shouldn't** write your links like
  this:

  .. code-block:: rest

      A paragraph with `a wrong hyperlink`_.

      .. _a wrong hyperlink: http://example.com
  
  Rather, you should write them like this:

  .. code-block:: rest

      A paragraph with `a correct hyperlink <link_>`_.

      .. _link: http://example.com
  
  Note the extra ``<link_>``.
  Not only does this form free you from having to repeat the entire text of your link,
  it's important for localization. Having these explicit link identifiers allows translators
  to translate the link text as well as rearrange links in a paragraph without messing
  up the link targets.

* For similar reasons, when using footnotes, don't use implicitly numbered footnotes (``[#]_``). Always
  specify an explicit footnote identifier, for example ``[#f1]_``. You can also use more
  explicit names if you prefer. Don't worry about having to renumber all footnotes if you
  insert a new one somewhere in-between others: the ordering and numbering of your footnotes in the
  final document is only determined by the order of the footnote bodies at the end of the
  source document.

* Always use appropriate semantic markup, where available, rather than just styling
  things with simple markup elements. So for instance, when referring to an executable
  program, use the ``program`` role (``:program:`speedcrunch```).

* Don't write the manual as if just for one specific version of SpeedCrunch. Instead, document when
  a feature was introduced or what changed between SpeedCrunch versions. This keeps the latest
  version of the documentation relevant even for older SpeedCrunch versions and helps avoid
  confusion about how the versions differ. Use the |versionadded|_, |versionchanged|_ and
  |deprecated|_ directives with the SpeedCrunch version to mark the differences between
  versions.

.. |versionadded| replace:: ``versionadded``
.. |versionchanged| replace:: ``versionchanged``
.. |deprecated| replace:: ``deprecated``
.. _versionadded: http://www.sphinx-doc.org/en/stable/markup/para.html#directive-versionadded
.. _versionchanged: http://www.sphinx-doc.org/en/stable/markup/para.html#directive-versionchanged
.. _deprecated: http://www.sphinx-doc.org/en/stable/markup/para.html#directive-deprecated



The SpeedCrunch Domain
----------------------

A *domain* in the context of Sphinx is simply a set of reST markup elements to describe
related objects. [#f1]_ The SpeedCrunch domain (short name: ``sc``) provides
directives to document SpeedCrunch "objects", i.e. things like functions or constants. These
are documented below with their full name, i.e. the ``sc:`` prefix. However, all documents
by default use the ``sc`` domain, so it's not actually necessary to specify the
prefix when using these directives. For example, to refer to a function, simply use
``:func:`function_name```.

All of these directives automatically create all required index entries. This includes
the entries in the main index, the entries in the function index and the necessary
entries for the context help feature.


.. rst:directive:: .. sc:function:: function_name(arg; arg [; arg; arg; ...])

   Document a SpeedCrunch function. Specify the function's signature as above, with
   parameters separated by ``;`` and optional parameters enclosed in square brackets. In the body
   of the directive, you can use the following fields to describe specific aspects
   of the function:

   * ``param``, ``parameter``, ``arg``, ``argument`` to describe a parameter.
   * ``returns``, ``return`` to describe the return value.

   A complete example looks like this:

   .. code-block:: rest

       .. sc:function:: foo(a; b [; c])

          Textual description of what the function does.

          :param a: The first parameter. Describe its role, its
                    range of values, whatever you can think of.
          :param b: The second parameter.
          :param c: The third, optional parameter.
          :returns: Describe the return value.


.. rst:directive:: .. sc:constant:: constant_name

   Document a built-in constant.


.. rst:role:: sc:func

   Link to the documentation for a built-in function. You can optionally specify
   the parentheses after the function name, but they aren't necessary; in the output,
   the link text will always include the parentheses for consistency. In text, this
   is used like this:

   .. code-block:: rest

       This sentence includes a reference to :sc:func:`sin`.


.. rst:role:: sc:const

   Link to the documentation for a constant. This is used in text like this:

   .. code-block:: rest

       This sentence includes a reference to :sc:const:`pi`.



Tooling
-------

While writing the documentation, you can use the usual Sphinx tools to preview your
progress. From the manual directory, run ``make html`` [#f2]_ to generate the HTML
documentation in the :file:`_build/html` directory. This will use a different theme
than the integrated manual, but it's still useful for checking your markup. If you
need more control, you can also invoke :program:`sphinx-build` directly; see the
Sphinx documentation on `sphinx-build <sphinx-build_>`_ for detailed usage
instructions. You can also use `sphinx-autobuild <sphinx-autobuild_>`_ to automatically
update the HTML documents whenever you modify the sources.

.. _sphinx-build: http://sphinx-doc.org/en/stable/invocation.html#invocation-of-sphinx-build
.. _sphinx-autobuild: https://github.com/GaretJax/sphinx-autobuild

To generate the documentation as it would be for the integrated manual, you need to
pass the ``sc_bundled_docs`` tag to :program:`sphinx-build` using the |sphinx-build-t|_.
Most notably, this switches the theme to the one used for the bundled documentation. In general,
you shouldn't need to write documentation that's different between the integrated manual
and stand-alone documentation. Still, if you need to distinguish between these, you
can check for the presence of the ``sc_bundled_docs`` tag. See the
`Sphinx documentation on tags <tags_>`_ for more information on how to use these.

.. |sphinx-build-t| replace:: ``-t`` flag
.. _sphinx-build-t: http://sphinx-doc.org/en/stable/invocation.html#cmdoption-sphinx-build-t
.. _tags: http://sphinx-doc.org/en/stable/markup/misc.html#tags

:program:`doc-tool` is the custom Python script used to automate certain
documentation-related tasks. It can be found in the root directory of the manual in
the SpeedCrunch source repository (:file:`doc/manual/doc-tool.py`).
Its main purpose is to generate the documentation for any
supported target language with a single command; it is used during the SpeedCrunch build
process to handle this and other aspects of the documentation build. In addition, :program:`doc-tool`
has shortcuts for certain maintainer tasks. To get an overview of supported options and commands,
run ``doc-tool --help``.



.. rubric:: Footnotes

.. [#f1] See the Sphinx documentation `for more information on domains <domains_>`_.
.. [#f2] Or ``./make html`` in Windows PowerShell.

.. _domains: http://sphinx-doc.org/en/stable/domains.html

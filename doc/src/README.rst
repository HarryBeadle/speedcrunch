The following tools are required for building the documentation:

* Python 2.7 or 3.4 or later
* `Sphinx <sphinx_>`_ 1.3 or later
* `the Quark theme <quark_>`_ 0.2 or later

It is recommended to install Python and pip from `the Python website <python_>`_.
or the package manager of your choice, then run: ::

    pip install "sphinx>=1.3" "quark-sphinx-theme>=0.2"

When working with localization, you may additionally require:

* `sphinx-intl <sphinx-intl_>`_
* `Transifex CLI client <transifex-cli_>`_

.. _python: http://www.python.org
.. _sphinx: http://sphinx-doc.org
.. _sphinx-intl: https://pypi.python.org/pypi/sphinx-intl
.. _transifex-cli: http://docs.transifex.com/client/
.. _quark: https://pypi.python.org/pypi/quark-sphinx-theme

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
pass the ``sc_bundled_docs`` tag to :program:`sphinx-build` using the command-line flag |sphinx-build-t|_.
Most notably, this switches the theme to the one used for the bundled documentation. In general,
you shouldn't need to write documentation that's different between the integrated manual
and stand-alone documentation. Still, if you need to distinguish between these, you
can check for the presence of the ``sc_bundled_docs`` tag. See the
`Sphinx documentation on tags <tags_>`_ for more information on how to use these.

.. |sphinx-build-t| replace:: ``-t``
.. _sphinx-build-t: http://sphinx-doc.org/en/stable/invocation.html#cmdoption-sphinx-build-t
.. _tags: http://sphinx-doc.org/en/stable/markup/misc.html#tags

:program:`doc-tool` is the custom Python script used to automate certain
documentation-related tasks. It can be found in the root directory of the manual in
the SpeedCrunch source repository (:file:`doc/src/doc-tool.py`).
Its main purpose is to generate the documentation for any
supported target language with a single command. In addition, :program:`doc-tool`
has shortcuts for certain maintainer tasks. To get an overview of supported options and commands,
run ``doc-tool --help``.

* To generate ``.pot`` files for translation, run ``doc-tool.py extract-strings``. This will use :program:`sphinx`' ``gettext`` builder to crawl any reST files for translatable strings. Custom extensions to :prog:`sphinx` will also be serached for translatable strings.
* To update the ``.po`` files (synchronize them with the ``.pot``), run ``doc-tool.py update-translations``. Not used since this will be handled by Transifex (?).
* To actually build the docs, run either ``doc-tool.py build-standalone-docs`` or ``doc-tool.py build-bundled-docs``.

The build processes of the documentation and SpeedCrunch itself have been separated. Indeed, when building SpeedCrunch itself, the documentation will be fetched from :file:`doc/build_html_embedded/`. Note that this is not the default output directory of :program:`doc-tool`!

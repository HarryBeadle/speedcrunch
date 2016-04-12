.. highlight:: shell


.. _building-from-source:

Building from Source
====================

The SpeedCrunch source code is maintained on `Bitbucket`_ in a Git repository. Clone
the repository onto your machine by running the following command::

    git clone https://bitbucket.org/heldercorreia/speedcrunch.git

.. _Bitbucket: https://bitbucket.org/heldercorreia/speedcrunch


Dependencies
------------

The following software is required to compile SpeedCrunch from source:

 * a supported C++ compiler, for example:

   - `GCC <gcc_>`_, version 4.8 or later
   - `Microsoft Visual C++ <msvc_>`_ 2013 or later

 * `Qt <qt_>`_, version 5.2 or later
 * `CMake <cmake_>`_, version 2.8.12 or later
 
.. _gcc: https://gcc.gnu.org
.. _msvc: http://visualstudio.com
.. _qt: http://qt.io
.. _cmake: http://cmake.org


On Ubuntu or Debian, the following command will install these dependencies::

    sudo apt-get install build-essential cmake qtbase5-dev qttools5-dev qttools5-dev-tools


Building
--------

At a high level, the build process consists of these steps::

    cd <build directory>
    cmake <SpeedCrunch source directory>/src
    make

The build directory can be any empty directory. While it is possible to build SpeedCrunch
inside the source tree, it is discouraged. On Windows, it will usually be necessary
to run these commands from a command prompt that is set up for the compiler you're using.

In addition, the final ``make`` command to invoke may differ depending on the platform
and the CMake generator used; for example, the command for MSVC with the
``NMake Makefiles`` generator is ``nmake``. Other
generators may require yet other commands while some generators targetting IDEs like
Visual Studio may not even have a command-line interface in this fashion. Consult the
`CMake documentation <cmake_doc_generators_>`_ for more information on available generators
and how to use them. From here on, consider any command like ``make <target-name>``, substitute
the appropriate command or other method to build a certain target for your toolchain.

.. _cmake_doc_generators: https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html

When building against a Qt version that is not the system default Qt installation,
it will be necessary to point CMake towards the
Qt installation to use. This can be achieved by setting the ``CMAKE_PREFIX_PATH``
environment variable to the root directory of the Qt installation you want to use. [#f1]_

The SpeedCrunch build supports several additional configuration variables. These can
either be set when invoking CMake using the form ``cmake -D<VAR>=<VALUE>`` or interactively
via the CMake GUI tool.

.. index::
   pair: PORTABLE_SPEEDCRUNCH; CMake variable

.. describe:: PORTABLE_SPEEDCRUNCH

   When set to ``on``, SpeedCrunch is built in portable mode: all settings will be
   stored in the same directory as the executable.


Installing
----------

To install SpeedCrunch after building, run ``make install`` in the
build directory. Note that this step may require administrator/root privileges.

To customize the installation directory, set the following CMake variable at configuration
time:

.. index::
   pair: CMAKE_INSTALL_PREFIX; CMake variable

.. describe:: CMAKE_INSTALL_PREFIX

   Set the installation prefix for the ``install`` target.


Building the Documentation
--------------------------

By default, building SpeedCrunch does not actually rebuild the embedded manual; instead,
a prebuilt copy of the HTML files that is bundled with the sources is included in the application. This keeps
the number of dependencies required to build SpeedCrunch to a minimum. If you
want to rebuild the manual, the following additional dependencies are required:

 * `Python <py_>`_, version 3.4 or later
 * `Sphinx <sphinx_>`_, version 1.3 or later
 * the `Quark Sphinx theme <quark_>`_, version 0.2 or later

.. _py: http://python.org
.. _sphinx: http://sphinx-doc.org
.. _quark: https://pypi.python.org/pypi/quark-sphinx-theme


The following variables can be used to control the manual build behavior:

.. _var_rebuild_manual:

.. index::
   pair: REBUILD_MANUAL; CMake variable

.. describe:: REBUILD_MANUAL

   Set this to true to automatically rebuild the manual as part of the SpeedCrunch build.
   Otherwise the bundled prebuilt copy is used instead. Note that this setting does not update
   the prebuilt manual; see :ref:`the documentation guide <update_prebuilt_manual>` on how
   to do that.


.. index::
   pair: PYTHON_EXECUTABLE; CMake variable

.. describe:: PYTHON_EXECUTABLE

   The path of the Python executable used for running additional build scripts. Normally,
   this is determined automatically and doesn't need to be changed.


.. index::
   pair: QCOLLECTIONGENERATOR_EXECUTABLE; CMake variable

.. describe:: QCOLLECTIONGENERATOR_EXECUTABLE

   The path to the :program:`qcollectiongenerator` program used to generate the bundled
   documentation. Normally, this is automatically set to the :program:`qcollectiongenerator`
   binary included with Qt and doesn't need to be changed.


.. index::
   pair: SPHINX_EXECUTABLE; CMake variable

.. describe:: SPHINX_EXECUTABLE

   The path to the :program:`sphinx-build` executable. This is often determined
   automatically, but it may be necessary to override it in some cases.


Creating Windows Installers
---------------------------

On Windows, an installer can be generated after building by running ``make package``
from the build directory. This will create a fully self-contained installer program.
For this to work, `NSIS <nsis_>`_ needs to be installed.

.. _nsis: http://nsis.sourceforge.net


Using Qt Creator
----------------

To build SpeedCrunch with `Qt Creator <qtc_>`_, simply open :file:`src/CMakeLists.txt` in the
IDE. Configuration variables can be specified in the :guilabel:`Run CMake` dialog,
using the ``-D<VAR>=<VALUE>`` syntax. Since Qt Creator handles the Qt setup for you,
you don't have to point the build system at a Qt installation and compiler; simply select
the desired toolchain and Qt version in Qt Creator.

.. _qtc: http://qt.io/ide



.. rubric:: Footnotes

.. [#f1] For example, on Windows this is often a path like ``C:/Qt/<Qt version>/<compiler>``, depending
         on where you installed Qt.

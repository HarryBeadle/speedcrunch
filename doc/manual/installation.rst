Installation
============

Microsoft Windows
-----------------

SpeedCrunch for Windows requires Windows Vista or newer (32-bit and 64-bit versions).
You can use choose either to use either the installer or the portable
version of SpeedCrunch. Both of these are available from `the SpeedCrunch website <sc_>`_.

.. _sc: http://speedcrunch.org

The installer installs SpeedCrunch on your system and creates an entry in the menu
to launch it. This process requires administrator rights.

Alternatively, you may use the portable version of SpeedCrunch, which is distributed
as a ``.zip`` archive. This version requires no installation, simply extract it to
a folder of your choice and run :program:`speedcrunch.exe`. In addition, this version of
the software is fully portable: it stores all its settings and user data next to the
executable so the entire directory can be copied to e.g. another computer and run
there.


Apple OS X
----------

TODO: OSX installation instructions


Linux
-----

Since the large number and variety of Linux distributions makes it impossible to
consider all of them here, this section only describes the SpeedCrunch installation
process for the most popular ones. If you use another distribution, we expect that
you will know better than us how to get software for your system.

Ubuntu & Debian
+++++++++++++++

SpeedCrunch is available from the package repositories on both Ubuntu and Debian. It can be installed
from the Ubuntu Software Centre or using the command line::

    sudo apt-get install speedcrunch

If you wish to use bleeding-edge development versions of SpeedCrunch on Ubuntu, you can install
these from the `SpeedCrunch Daily PPA <ppa_>`_. This PPA contains SpeedCrunch packages that
are regularly built from the SpeedCrunch development branch.

.. _ppa: https://code.launchpad.net/~fkrull/+archive/ubuntu/speedcrunch-daily

.. warning::

   Development builds of SpeedCrunch may not be as stable or well-tested as release version
   and may contain changes that break your session history. Use them at your own risk.


Building from Source
--------------------

.. Dependencies
.. ++++++++++++

The SpeedCrunch source code is maintained on `Bitbucket`_ in a Git repository. Clone
the repository onto your machine by running the following command::

    git clone https://bitbucket.org/heldercorreia/speedcrunch.git

.. _Bitbucket: https://bitbucket.org/heldercorreia/speedcrunch


Dependencies
++++++++++++

The following software is required to compile SpeedCrunch from source:

 * a supported C++ compiler, for example:

   - `GCC <gcc_>`_, version 4.8 or later
   - `Microsoft Visual C++ <msvc_>`_ 2013 or later

 * `Qt <qt_>`_, version 5.2 or later
 * `CMake <cmake_>`_, version 2.8.12 or later
 * `Python <py_>`_, either version 2.7 or version 3.4 or later
 * `Sphinx <sphinx_>`_, version 1.3 or later
 * the `Quark Sphinx theme <quark_>`_, version 0.2 or later

.. _gcc: https://gcc.gnu.org
.. _msvc: http://visualstudio.com
.. _qt: http://qt.io
.. _cmake: http://cmake.org
.. _py: http://python.org
.. _sphinx: http://sphinx-doc.org
.. _quark: https://pypi.python.org/pypi/quark-sphinx-theme


On Ubuntu or Debian, the following set of commands will install these dependencies::

    sudo apt-get install  build-essential cmake python3 python3-pip \
        qtbase5-dev qttools5-dev qttools5-dev-tools
    sudo pip3 install sphinx>=1.3 quark-sphinx-theme>=0.2


Building
++++++++

At a high level, the build process consists of these steps::

    cd <build directory>
    cmake <SpeedCrunch source directory>/src
    make

The build directory can be any empty directory. While it is possible to build SpeedCrunch
inside the source tree, it is discouraged.

On Windows, it will usually be necessary to run these commands from a command prompt
that is set up for the compiler you're using. In addition, the final ``make`` command
to invoke may be different depending on the compiler; for MSVC, it is ``nmake``.

When building against a Qt version that is not the system default Qt installation,
it will be necessary to point CMake towards the
Qt installation to use. This can be achieved by setting the ``CMAKE_PREFIX_PATH``
environment variable to the root directory of the Qt installation you want to use. [#f1]_

The SpeedCrunch build supports several additional configuration variables. These can
either be set when invoking CMake using the form ``cmake -D<VAR>=<VALUE>`` or interactively
via the CMake GUI tool.

.. index:: CMake variable; PORTABLE_SPEEDCRUNCH

.. describe:: PORTABLE_SPEEDCRUNCH

   When set to ``on``, SpeedCrunch is built in portable mode: all settings will be
   stored in the same directory as the executable.


.. index:: CMake variable; PYTHON_EXECUTABLE

.. describe:: PYTHON_EXECUTABLE

   The path of the Python executable used for running additional build scripts. Normally,
   this is determined automatically and doesn't need to be changed.


.. index:: CMake variable; QCOLLECTIONGENERATOR_EXECUTABLE

.. describe:: QCOLLECTIONGENERATOR_EXECUTABLE

   The path to the :program:`qcollectiongenerator` program used to generate the bundled
   documentation. Normally, this is automatically set to the :program:`qcollectiongenerator`
   binary included with Qt and doesn't need to be changed.


.. index:: CMake variable; SPHINX_EXECUTABLE

.. describe:: SPHINX_EXECUTABLE

   The path to the :program:`sphinx-build` executable. This can often be determined
   automatically, but it may be necessary to override it in some cases.


Installing
++++++++++

To install SpeedCrunch after building, run ``make install`` (or equivalent) in the
build directory. Note that this step may require administrator/root privileges.

To customize the installation directory, set the following CMake variable at configuration
time:

.. index:: CMake variable; CMAKE_INSTALL_PREFIX

.. describe:: CMAKE_INSTALL_PREFIX

   Set the installation prefix for the ``install`` target.


Creating Windows Installers
+++++++++++++++++++++++++++

On Windows, an installer can be generated after building by running ``make package``
from the build directory. This will create a fully self-contained installer program.
For this to work, `NSIS <nsis_>`_ needs to be installed.

.. _nsis: http://nsis.sourceforge.net


Using Qt Creator
++++++++++++++++

To build SpeedCrunch with `Qt Creator <qtc_>`_, simply open :file:`src/CMakeLists.txt` in the
IDE. Configuration variables can be specified in the :guilabel:`Run CMake` dialog,
using the ``-D<VAR>=<VALUE>`` syntax. Since Qt Creator handles the Qt setup for you,
you don't have to point the build system at a Qt installation and compiler; simply select
the desired toolchain and Qt version in Qt Creator.

.. _qtc: http://qt.io/ide



.. rubric:: Footnotes

.. [#f1] For example, on Windows this is often a path like ``C:/Qt/<Qt version>/<compiler>``, depending
         on where you installed Qt.

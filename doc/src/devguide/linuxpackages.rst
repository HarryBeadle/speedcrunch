Building Linux Binary Packages
==============================

The SpeedCrunch repository includes scripts to build generic Linux binary packages that are
intended to be compatible with as many Linux distributions as possible. The packages are
built in a controlled environment using a relatively old Linux distribution
(currently `CentOS 6 x86-64 <centos_>`_ with the `Red Hat Developer Toolset 2.0 <devtoolset_>`_)
and include their own copy of the Qt libraries.
For each architecture (x86 and x86-64), three packages are generated: a Debian package (``*.deb``),
an RPM package and a generic tarball.

.. _centos: http://centos.org
.. _devtoolset: https://access.redhat.com/documentation/en-US/Red_Hat_Developer_Toolset/2/html/2.0_Release_Notes/index.html

To build the Linux packages, the following software is required:

* `Vagrant <vagrant_>`_
* `VirtualBox <vbox_>`_

.. _vagrant: http://vagrantup.com
.. _vbox: http://virtualbox.org

From the directory :file:`pkg/cross-linux` in the SpeedCrunch source repository,
run ``vagrant up`` to boot and provision the build VM. Then, log into the virtual
machine using ``vagrant ssh`` [#f1]_ and run the following commands:

.. code-block:: shell

    /vagrant/build-qt.sh # to build Qt, only needs to be done once
    /vagrant/build-speedcrunch.sh

Afterwards, the packages can be found in :file:`pkg/cross-linux/dist`.

Depending on your system configuration, it may be necessary to adjust the number of CPU cores
allotted to the VM by editing the ``vb.cpus = ...`` line in :file:`pkg/cross-linux/Vagrantfile`.
To change the package version number, edit the ``RELEASE_VERSION=...`` line in :file:`pkg/cross-linux/build-speedcrunch.sh`.

Rather than using the Vagrant configuration, the scripts can also be run in any other
CentOS 6 x86-64 installation. However, it will be necessary to adjust the paths in
:file:`build-speedcrunch.sh` to match the environment.


.. seealso::

   * `GCC ABI Policy and Guidelines <gccabi_>`_
   * `glibc API/ABI Compatbility Tracker <glibccompat_>`_

.. _gccabi: https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html
.. _glibccompat: http://abi-laboratory.pro/tracker/timeline/glibc/


.. rubric:: Footnotes

.. [#f1] On Windows, use `PuTTY <putty_>`_ and `vagrant-multi-putty <vagrantmultiputty_>`_.

.. _putty: http://www.chiark.greenend.org.uk/~sgtatham/putty/
.. _vagrantmultiputty: https://github.com/nickryand/vagrant-multi-putty

# SpeedCrunch
SpeedCrunch is a high-precision scientific calculator. It features a syntax-highlighted scrollable display and is designed to be fully used via keyboard. Some distinctive features are auto-completion of functions and variables, a formula book, and quick insertion of constants from various fields of knowledge. Available for Windows, OS X, and Linux in a number of languages.

![capture.png](https://bitbucket.org/repo/dR7BnG/images/3654665019-capture.png)

## Building
To build SpeedCrunch, you need:

- Qt 5.2 or later
- optionally: CMake 2.8.12 or later

To build from source, use either CMake or QMake to build the project:

    cd src && cmake . && make

or

    cd src && qmake speedcrunch.pro && make
    
In order to have your application settings stored in the same location as
the executable for portability (e.g. running from a USB drive or a directory
without requiring installation and special permissions), configure the project
as below:

    cmake . -DPORTABLE_SPEEDCRUNCH=on

or

    qmake "DEFINES+=SPEEDCRUNCH_PORTABLE" speedcrunch.pro

### Documentation
By default, building the application does not rebuild the manual that's included
with it; instead, a prebuilt copy is used to minimize dependencies. If you wish
to also rebuild the manual, you will need the following additional dependencies:

- Python 3.4 or later
- [Sphinx](http://sphinx-doc.org) 1.3 or later
- [the Quark theme](https://pypi.python.org/pypi/quark-sphinx-theme) 0.2 or later

Then, build using CMake and configure the project with the `REBUILD_MANUAL`
option:

    cmake . -DREBUILD_MANUAL=on

For more in-depth information on building SpeedCrunch, consult the
"Installation" chapter of the manual.

## Contributing
- Report bugs or request features in the [issue tracker](https://bitbucket.org/heldercorreia/speedcrunch/issues).
- Add or improve a [translation](https://www.transifex.com/projects/p/speedcrunch/).
- Send a message to the [forum](https://groups.google.com/group/speedcrunch).
- Follow the news on the [blog](http://speedcrunch.blogspot.com).

## License
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
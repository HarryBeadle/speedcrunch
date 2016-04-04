#!/bin/sh
set -e

BASE_DIR=~
QT_SOURCE_DIR=$BASE_DIR/qt-source

COMMON_CONFIGURE_ARGS="-opensource -confirm-license \
    -nomake tests -nomake examples \
    -skip qtwebengine -skip qtwebview -no-qml-debug \
    -release -static -no-sse3 -no-opengl \
    -qt-xcb -qt-xkbcommon-x11 -qt-pcre -qt-zlib -qt-harfbuzz -qt-libpng \
    -fontconfig -dbus-linked -qt-sql-sqlite"

CONFIGURE_ARGS_32="-xplatform linux-g++-32 -pkg-config"
CONFIGURE_ARGS_64="-platform linux-g++-64"

function build_qt {
    ARCH=$*
    echo "Building for '$ARCH'..."
    BUILD_DIR=$BASE_DIR/qt$ARCH-build
    INSTALL_DIR=$BASE_DIR/qt$ARCH-install
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    scl enable devtoolset-2 "bash <<END
    $QT_SOURCE_DIR/configure $COMMON_CONFIGURE_ARGS $(eval echo \$CONFIGURE_ARGS_$ARCH) -prefix $INSTALL_DIR
    make -j$(nproc --all)
    make install
END"
}

build_qt 32
build_qt 64

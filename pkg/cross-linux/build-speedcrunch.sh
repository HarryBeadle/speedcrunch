#!/bin/sh
set -e

BASE_DIR=~
SPEEDCRUNCH_SOURCE_DIR=/speedcrunch-source
OUTPUT_DIR=/vagrant/build

function build_speedcrunch {
    ARCH=$*
    echo "Building for '$ARCH'..."
    BUILD_DIR=$BASE_DIR/speedcrunch$ARCH-build
    QT_INSTALL_DIR=$BASE_DIR/qt$ARCH-install
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    scl enable devtoolset-2 "bash <<END
    $QT_INSTALL_DIR/bin/qmake $SPEEDCRUNCH_SOURCE_DIR/src 'CONFIG-=debug'
    make -j$(nproc --all)
    strip speedcrunch
    mkdir -p $OUTPUT_DIR/speedcrunch-$ARCH
    cp speedcrunch $OUTPUT_DIR/speedcrunch-$ARCH/
END"
}

build_speedcrunch 32
build_speedcrunch 64

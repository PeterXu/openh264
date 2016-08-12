#!/bin/bash

if [ "$1" = "clean" ]; then
    make clean
    find . -name *.o -exec rm -f {} \;
    exit 0
fi

# arm/arm64
ARCH=${1:-arm}
make OS=android ARCH=${ARCH} NDKROOT=$ANDROID_NDK TARGET=android-15 NDKLEVEL=21 PREFIX=out/$ARCH install
#rm -f out/lib/libopenh264.a

exit 0

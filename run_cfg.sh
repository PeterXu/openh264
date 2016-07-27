#!/bin/bash

ARCH=${1:-arm}
make OS=android ARCH=arm NDKROOT=$ANDROID_NDK TARGET=android-22 PREFIX=out install
#rm -f out/lib/libopenh264.a

exit 0

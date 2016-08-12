#!/bin/bash

if [ "$1" = "clean" ]; then
    make clean
    find . -name *.o -exec rm -f {} \;
    exit 0
elif [ "$1" = "distclean" ]; then
    make clean
    find . -name *.o -exec rm -f {} \;
    rm -rf out
    exit 0
fi

if [ "$1" = "arm64" ]; then
    ARCH=arm64
    ABI="arm64-v8a"
    API=21
else
    ARCH=arm
    ABI="armeabi-v7a"
    API=15
fi

make OS=android ARCH=${ARCH} NDKROOT=$ANDROID_NDK TARGET=android-15 NDKLEVEL=${API} PREFIX=out/$ABI install

exit 0

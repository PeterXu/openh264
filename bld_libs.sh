mkdir -p libs

clean_objs() {
	make clean
	rm -f ./codec/parser/src/h264parser.o
	rm -f ./codec/console/prs/src/h264prs.o
	rm -f ./codec/common/src/cpu-features.o
}

bld_none() {
    echo "usage: $0 android|ios|mac|win"
}

android_dname() {
	if [ "$1" = "arm" ]; then
		echo "armeabi-v7a"
	elif [ "$1" = "arm64" ]; then
		echo "arm64-v8a"
    else
	    echo "$1"
	fi
}

bld_android() {
	dpath="libs/android"
	mkdir -p $dpath
	archs="arm arm64 x86 x86_64"
	for arch in $archs; do
		clean_objs
		make OS=android NDKROOT=/usr/local/android-ndk TARGET=android-23 ARCH=$arch NDKLEVEL=23
		dname=$(android_dname $arch)
		mkdir -p $dpath/$dname
		cp -f libopenh264.so $dpath/$dname
	done
}

bld_ios() {
	dpath="libs/ios"
	mkdir -p $dpath
	archs="armv7 arm64"
	lipos=""
	for arch in $archs; do
		clean_objs
		make OS=ios ARCH=$arch
		mkdir -p $dpath/$arch
		cp -f libopenh264.a $dpath/$arch
		lipos="$lipos -arch $arch $dpath/$arch/libopenh264.a"
	done
	lipo $lipos -output $dpath/libopenh264.a -create
}

bld_mac() {
	dpath="libs/mac"
	mkdir -p $dpath
	clean_objs
	make ARCH=x86_64
	cp -f libopenh264.dylib $dpath
}

bld_win() {
	export PATH="$PATH:/c/Program Files (x86)/Microsoft Visual Studio 11.0/VC/bin:/c/Program Files (x86)/Microsoft Visual Studio 11.0/Common7/IDE"
	export INCLUDE="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include;C:\Program Files (x86)\Windows Kits\8.0\Include\um;C:\Program Files (x86)\Windows Kits\8.0\Include\shared"
	export LIB="C:\Program Files (x86)\Windows Kits\8.0\Lib\Win8\um\x86;C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\lib"

	dpath="libs/win"
	mkdir -p $dpath
	clean_objs
	make OS=msvc
	cp -f libopenh264.dll $dpath
}


func="none"
[ $# -eq 1 ] && func="$1"

if [ "$func" = "all" ]; then
	bld_mac
	bld_ios
	bld_android
else
	bld_$func
fi

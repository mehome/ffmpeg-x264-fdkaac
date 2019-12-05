#!/bin/bash

ARCH=$1


source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/libx264
echo "LIBS_DIR="$LIBS_DIR
echo "ANDROID_NDK_ROOT="$ANDROID_NDK_ROOT

cd x264

PREFIX=$LIBS_DIR/$AOSP_ABI
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/darwin-x86_64
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/$TOOLNAME_BASE-

export CXX="${CROSS_PREFIX}g++ --sysroot=${SYSROOT}"
export LDFLAGS=" -L$SYSROOT/usr/lib  $CFLAGS "
export CXXFLAGS="$CFLAGS -std=c++11"
export CFLAGS=$CFLAGS
export CC="${CROSS_PREFIX}gcc --sysroot=${SYSROOT}"
export AR="${CROSS_PREFIX}ar"
export LD="${CROSS_PREFIX}ld"
export RANLIB="${CROSS_PREFIX}ranlib"
export AS="${CROSS_PREFIX}gcc"
export STRIP="${CROSS_PREFIX}strip"



./configure --prefix=$PREFIX \
--enable-shared \
--enable-static \
--enable-pic \
--enable-neon \
--enable-strip \
--disable-asm \
--disable-opencl \
--disable-cli \
--host=$HOST \
--cross-prefix=$CROSS_PREFIX \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic" \
--extra-ldflags="-Wl,-soname libx264.so"


make clean

make -j4
make install

cd ..

echo "================== 修改lib======================================"
#遍历文件移除软连so
for file in `ls $PREFIX/lib`
do
 if [ -L $PREFIX/lib/$file ]
 then
    echo $PREFIX/lib/$file" 是软连"
    rm $PREFIX/lib/$file
 else
    echo $PREFIX/lib/$file
 fi
done

#将带版本号的so 移除版本号
./patchelf --set-soname libx264.so $PREFIX/lib/libx264.so.*

if [ -f $PREFIX/lib/libx264.so ]
then
   rm $PREFIX/lib/libx264.so
else
   echo $PREFIX/lib/$file
fi
cp $PREFIX/lib/libx264.so.*  $PREFIX/lib/libx264.so
rm $PREFIX/lib/libx264.so.*
echo "rm $PREFIX/lib/libx264.so.*"
echo "==============================================================="

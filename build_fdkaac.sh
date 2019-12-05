#!/bin/bash
#如果中途发生错误就退出
set -e

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/libfdk-aac
echo "LIBS_DIR="$LIBS_DIR

cd fdk-aac-0.1.6


PREFIX=$LIBS_DIR/$AOSP_ABI
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/darwin-x86_64
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/$TOOLNAME_BASE-

CFLAGS=""

FLAGS="--host=$HOST --target=android --disable-asm "

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



./configure $FLAGS \
--enable-pic \
--enable-strip \
--prefix=$PREFIX

$ADDITIONAL_CONFIGURE_FLAG

make clean


make -j8
make install

cd ..

echo "================== 修改lib======================================"
#遍历文件移除软连so
for file in `ls $PREFIX/lib`
do
 if [ -L $PREFIX/lib/$file ]
 then
    echo $PREFIX/lib/$file" 是软连 需移除"
    rm $PREFIX/lib/$file
 else
    echo $PREFIX/lib/$file
 fi
done

#处理带版本号的so
if [ -f $PREFIX/lib/libfdk-aac.so ]
then
   #不做处理
else
   echo $PREFIX/lib/libfdk-aac.so.*
   #将带版本号的so 移除版本号
   ./patchelf --set-soname libfdk-aac.so $PREFIX/lib/libfdk-aac.so.*
   #拷贝 libfdk-aac.so.* 为libfdk-aac.so
   cp $PREFIX/lib/libfdk-aac.so.*  $PREFIX/lib/libfdk-aac.so
fi
# 移除libfdk-aac.so.*
rm $PREFIX/lib/libfdk-aac.so.*
echo "==============================================================="

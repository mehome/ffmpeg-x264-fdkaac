#!/bin/bash

ARCH=$1
echo "start build ffmpeg-3.3.9 for "$ARCH

source config.sh $ARCH
NOW_DIR=$(cd `dirname $0`; pwd)
LIBS_DIR=$NOW_DIR/libs
echo "LIBS_DIR="$LIBS_DIR

cd ffmpeg-3.3.9

PREFIX=$LIBS_DIR/ffmpeg-x264-fdkaac-merge/$AOSP_ABI
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/darwin-x86_64
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/$TOOLNAME_BASE-


FDK_INCLUDE=$LIBS_DIR/libfdk-aac/$AOSP_ABI/include
FDK_LIB=$LIBS_DIR/libfdk-aac/$AOSP_ABI/lib
X264_INCLUDE=$LIBS_DIR/libx264/$AOSP_ABI/include
X264_LIB=$LIBS_DIR/libx264/$AOSP_ABI/lib

echo "SYSROOT="$SYSROOT
echo "CROSS_PREFIX="$CROSS_PREFIX
echo "FDK_LIB="$FDK_LIB
echo "X264_LIB="$X264_LIB
echo "PREFIX="$PREFIX
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$FDK_LIB/pkgconfig
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$X264_LIB/pkgconfig


./configure \
--prefix=$PREFIX \
--arch=$AOSP_ARCH \
--target-os=android \
--cc=$TOOLCHAIN/bin/$TOOLNAME_BASE-gcc \
--cross-prefix=$CROSS_PREFIX \
--nm=$TOOLCHAIN/bin/$TOOLNAME_BASE-nm \
--sysroot=$SYSROOT \
--extra-cflags="-I$X264_INCLUDE -I$FDK_INCLUDE " \
--extra-ldflags="-L$X264_LIB -L$FDK_LIB " \
--enable-cross-compile \
--disable-runtime-cpudetect \
--enable-asm \
--enable-neon \
--enable-gpl \
--enable-nonfree \
--enable-shared \
--disable-static \
--enable-small \
--enable-libx264 \
--enable-neon \
--enable-yasm \
--enable-libfdk_aac \
--disable-encoders \
--enable-encoder=libx264 \
--enable-encoder=mpeg4 \
--enable-encoder=libfdk_aac \
--disable-decoders \
--enable-decoder=aac \
--enable-decoder=aac_latm \
--enable-decoder=h264 \
--enable-decoder=mpeg4 \
--disable-demuxers \
--enable-demuxer=h264 \
--enable-demuxer=aac \
--enable-demuxer=mov \
--disable-muxers \
--enable-muxer=mov \
--enable-muxer=mp4 \
--enable-muxer=h264 \
--disable-parsers \
--enable-parser=aac \
--enable-parser=ac3 \
--enable-parser=h264 \
--enable-avresample \
--disable-protocols \
--enable-protocol=file \
--disable-debug \
--disable-doc \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-postproc \
--disable-avfilter \
--disable-avdevice \
--disable-symver

make clean
make -j4
make install

cd ..


#!/bin/bash

set -e

if [ -d ./libs/ ]
then
   rm -rf libs
else
   echo "libs not exist!"
fi
mkdir libs

if [ -d ./fdk-aac-0.1.6/ ]
then
   rm -rf ./fdk-aac-0.1.6
else
   echo "fdk-aac-0.1.6  not exist!"
fi
tar -xf fdk-aac-0.1.6.tar

if [ -d ./ffmpeg-3.3.9/ ]
then
   rm -rf ./ffmpeg-3.3.9
else
   echo "ffmpeg-3.3.9 not exist!"
fi
tar -xf ffmpeg-3.3.9.tar.xz

if [ -d ./x264/ ]
then
   rm -rf ./x264
else
   echo "x264 not exist!"
fi
tar -xf x264.zip


./build_fdkaac_all.sh
./build_x264_all.sh
./build_ffmpeg_with_x264_fdkaac_all.sh

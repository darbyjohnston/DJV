#!/bin/sh

set -x

BUILD_TYPE=Release
if [ "$#" -eq 1 ]; then
    BUILD_TYPE=$1
fi

export FEATHER_TK_API=GLES_2
export TLRENDER_NET=OFF
export TLRENDER_JPEG=OFF
export TLRENDER_TIFF=OFF
export TLRENDER_STB=OFF
export TLRENDER_EXR=OFF
export TLRENDER_FFMPEG=ON
export TLRENDER_FFMPEG_MINIMAL=ON
export TLRENDER_USD=OFF
export TLRENDER_BMD=OFF
export TLRENDER_BMD_SDK=

sh DJV/etc/Linux/linux-build-gha.sh $BUILD_TYPE

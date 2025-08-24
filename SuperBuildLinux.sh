#!/bin/sh

set -x

BUILD_TYPE=Release
if [ "$#" -eq 1 ]; then
    BUILD_TYPE=$1
fi

export TLRENDER_NET=OFF
export TLRENDER_OCIO=ON
export TLRENDER_FFMPEG=ON
export TLRENDER_FFMPEG_MINIMAL=OFF
export TLRENDER_USD=OFF
export TLRENDER_BMD=OFF
export TLRENDER_BMD_SDK=
export FEATHER_TK_API=GL_4_1

sh DJV/etc/Linux/linux-build-gha.sh $BUILD_TYPE

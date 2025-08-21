#!/bin/bash

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
export FEATHER_TK_API=GL_4_1

bash DJV/etc/Linux/linux-build-gha.bat $BUILD_TYPE

cmake \
    -S DJV/etc/SuperBuild \
    -B superbuild-$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$PWD/install-$BUILD_TYPE \
    -DCMAKE_PREFIX_PATH=$PWD/install-$BUILD_TYPE \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build superbuild-$BUILD_TYPE -j $JOBS --config $BUILD_TYPE

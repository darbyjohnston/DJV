#!/bin/bash

set -x

BUILD_TYPE=$1

# Update packages
sudo apt-get update

# Install OpenGL support
sudo apt-get install xorg-dev libglu1-mesa-dev mesa-common-dev mesa-utils xvfb
xvfb-run glxinfo

# Install ALSA and PulseAudio support
sudo apt-get install libasound2-dev
sudo apt-get install libpulse-dev

# Build DJV
JOBS=4

cmake \
    -S DJV/etc/SuperBuild \
    -B superbuild-$BUILD_TYPE \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$PWD/install-$BUILD_TYPE \
    -DCMAKE_PREFIX_PATH=$PWD/install-$BUILD_TYPE \
    -DTLRENDER_NET=$TLRENDER_NET \
    -DTLRENDER_OCIO=$TLRENDER_OCIO \
    -DTLRENDER_FFMPEG=$TLRENDER_FFMPEG \
    -DTLRENDER_FFMPEG_MINIMAL=$TLRENDER_FFMPEG_MINIMAL \
    -DTLRENDER_USD=$TLRENDER_USD \
    -DFEATHER_TK_API=$FEATHER_TK_API
cmake --build superbuild-$BUILD_TYPE -j $JOBS --config $BUILD_TYPE

cmake \
    -S DJV \
    -B build-$BUILD_TYPE \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$PWD/install-$BUILD_TYPE \
    -DCMAKE_PREFIX_PATH=$PWD/install-$BUILD_TYPE \
    -DTLRENDER_NET=$TLRENDER_NET \
    -DTLRENDER_OCIO=$TLRENDER_OCIO \
    -DTLRENDER_FFMPEG=$TLRENDER_FFMPEG \
    -DTLRENDER_USD=$TLRENDER_USD \
    -DFEATHER_TK_API=$FEATHER_TK_API
cmake --build build-$BUILD_TYPE -j $JOBS --config $BUILD_TYPE

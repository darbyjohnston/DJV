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
mkdir build
cd build
cmake ../etc/SuperBuild \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$PWD/install \
    -DCMAKE_PREFIX_PATH=$PWD/install \
    -DTLRENDER_FFMPEG=$TLRENDER_FFMPEG \
    -DTLRENDER_USD=$TLRENDER_USD \
    -Ddtk_API=$dtk_API
cmake --build . -j 4 --config $BUILD_TYPE

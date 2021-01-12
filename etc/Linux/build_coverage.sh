#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-coverage-install/bin:$PATH
export DYLD_LIBRARY_PATH=$DJV_BUILD/DJV-coverage-install/lib:$DYLD_LIBRARY_PATH

mkdir -p DJV-coverage
cd DJV-coverage
cmake \
    ../DJV/etc/SuperBuild -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-coverage-install \
    -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-coverage-install \
    -DDJV_BUILD_TINY=TRUE \
    -DDJV_THIRD_PARTY_OPTIONAL=FALSE \
    -DDJV_GCOV=TRUE \
    -DDJV_GL_ES2=TRUE
cmake --build . -j 8


#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-install-coverage/bin:$PATH
export DYLD_LIBRARY_PATH=$DJV_BUILD/DJV-install-coverage/lib:$DYLD_LIBRARY_PATH

mkdir -p DJV-third-party-coverage
cd DJV-third-party-coverage
cmake \
    ../DJV/third-party \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-coverage \
    -DCMAKE_FIND_FRAMEWORK="LAST" \
    -DDJV_BUILD_TINY=TRUE \
    -DDJV_THIRD_PARTY_OPTIONAL=FALSE \
    -DDJV_GCOV=TRUE \
    -DDJV_GL_ES2=TRUE
cmake --build . -j 8
cmake --build . -j 8 --target install
cd ..

mkdir -p DJV-coverage
cd DJV-coverage
cmake \
    ../DJV -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-coverage \
    -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-install-coverage \
    -DCMAKE_FIND_FRAMEWORK="LAST" \
    -DDJV_BUILD_TINY=TRUE \
    -DDJV_THIRD_PARTY_OPTIONAL=FALSE \
    -DDJV_GCOV=TRUE \
    -DDJV_GL_ES2=TRUE
cmake --build . -j 8

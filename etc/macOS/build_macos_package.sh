#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-package-install/bin:$PATH
export DYLD_LIBRARY_PATH=$DJV_BUILD/DJV-package-install/lib:$DYLD_LIBRARY_PATH

mkdir -p DJV-package
cd DJV-package
cmake \
    ../DJV/etc/SuperBuild \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-package-install \
    -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-package-install \
    -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 8

cd DJV/src/DJV-build
cmake ../../../../../DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 8
cmake --build . -j 8 --target package

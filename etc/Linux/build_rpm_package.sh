#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-package-install/bin:$PATH
export LD_LIBRARY_PATH=$DJV_BUILD/DJV-package-install/lib:$LD_LIBRARY_PATH

mkdir -p DJV-package
cd DJV-package
cmake \
    ../DJV/etc/SuperBuild \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-package-install \
    -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-package-install
cmake --build . -j 8

cd DJV/src/DJV-build
cmake ../../../../../DJV -DCPACK_GENERATOR=RPM -DCPACK_PACKAGING_INSTALL_PREFIX=/usr/local/DJV2 -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 8
cmake --build . -j 8 --target package
cmake ../../../../../DJV -DCPACK_GENERATOR=TGZ -DCPACK_PACKAGING_INSTALL_PREFIX= -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 8
cmake --build . -j 8 --target package


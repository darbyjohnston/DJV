#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-install-Release/bin:$PATH
export LD_LIBRARY_PATH=$DJV_BUILD/DJV-install-Release/lib:$LD_LIBRARY_PATH

mkdir -p DJV-third-party-Release
cd DJV-third-party-Release
cmake ../DJV/third-party -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-Release
cmake --build . -j 8
cmake --build . -j 8 --target install
cd ..

mkdir -p DJV-Release
cd DJV-Release
cmake ../DJV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-Release -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-install-Release
cmake --build . -j 8

cmake ../DJV -DCPACK_GENERATOR=DEB -DCPACK_PACKAGING_INSTALL_PREFIX=/usr/local/DJV2 -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 8
cmake --build . --target package

cmake ../DJV -DCPACK_GENERATOR=TGZ -DCPACK_PACKAGING_INSTALL_PREFIX= -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 8
cmake --build . --target package


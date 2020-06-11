#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-install-Package/bin:$PATH
export DYLD_LIBRARY_PATH=$DJV_BUILD/DJV-install-Package/lib:$DYLD_LIBRARY_PATH

mkdir -p DJV-third-party-Package
cd DJV-third-party-Package
cmake ../DJV/third-party -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-Package -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 16
cmake --build . -j 16 --target install
cd ..

mkdir -p DJV-Package
cd DJV-Package
cmake ../DJV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-Package -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-install-Package -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 16

cmake ../DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 16
cmake --build . --target package

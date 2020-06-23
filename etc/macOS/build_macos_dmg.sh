#!/bin/sh

# Run this script in the parent directory of the repository.

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-install-package/bin:$PATH
export DYLD_LIBRARY_PATH=$DJV_BUILD/DJV-install-package/lib:$DYLD_LIBRARY_PATH

mkdir -p DJV-third-party-package
cd DJV-third-party-package
cmake ../DJV/third-party -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-package -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 8
cmake --build . -j 8 --target install
cd ..

mkdir -p DJV-package
cd DJV-package
cmake ../DJV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-package -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-install-package -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 8

cmake ../DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 8
cmake --build . -j 8 --target package

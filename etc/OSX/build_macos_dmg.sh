#!/bin/sh

git clone https://github.com/darbyjohnston/DJV.git

export DJV_BUILD=$PWD
export PATH=$DJV_BUILD/DJV-install-Release/bin:$PATH
export DYLD_LIBRARY_PATH=$DJV_BUILD/DJV-install-Release/lib:$DYLD_LIBRARY_PATH

mkdir -p DJV-third-party-Release
cd DJV-third-party-Release
cmake ../DJV/third-party -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-Release -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 16
cmake --build . -j 16 --target install
cd ..

mkdir -p DJV-Release
cd DJV-Release
cmake ../DJV -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$DJV_BUILD/DJV-install-Release -DCMAKE_PREFIX_PATH=$DJV_BUILD/DJV-install-Release -DCMAKE_FIND_FRAMEWORK="LAST"
cmake --build . -j 16

cmake ../DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . -j 16
cmake --build . --target package

git clone https://github.com/darbyjohnston/DJV.git

set DJV_BUILD=C:/dev
set PATH=%CD%\DJV-install-Release\bin;%PATH%

mkdir DJV-third-party-Release
cd DJV-third-party-Release
cmake ..\DJV\third-party -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%DJV_BUILD%/DJV-install-Release
cmake --build . --config Release -j
cmake --build . --config Release --target install
cd ..

mkdir DJV-Release
cd DJV-Release
cmake ..\DJV -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%DJV_BUILD%/DJV-install-Release -DCMAKE_INSTALL_PREFIX=%DJV_BUILD%/DJV-install-Release
cmake --build . --config Release -j

cmake ..\DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . --config Release -j
cmake --build . --config Release --target package

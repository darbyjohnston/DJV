REM Run this script in the parent directory of the repository.

set DJV_BUILD=C:/dev
set PATH=%CD%\DJV-install-package\bin;%PATH%

mkdir DJV-third-party-package
cd DJV-third-party-package
cmake ..\DJV\third-party -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%DJV_BUILD%/DJV-install-package
cmake --build . --config Release -j
cmake --build . --config Release -j --target install
cd ..

mkdir DJV-package
cd DJV-package
cmake ..\DJV -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%DJV_BUILD%/DJV-install-package -DCMAKE_INSTALL_PREFIX=%DJV_BUILD%/DJV-install-package
cmake --build . --config Release -j
cmake ..\DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . --config Release -j
cmake --build . --config Release -j --target package
cd ..

python DJV\etc\Windows\package_map.py DJV-package\package.map
MakeAppx pack /f DJV-package\package.map /p DJV-package/DJV_2.0.9.0_x64.appx /o

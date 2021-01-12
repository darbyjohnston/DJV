REM Run this script in the parent directory of the repository.

set DJV_BUILD=%CD%
set PATH=%CD%\DJV-package-install\bin;%PATH%

mkdir DJV-package
cd DJV-package
cmake ..\DJV\etc\SuperBuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%DJV_BUILD%/DJV-package-install -DCMAKE_INSTALL_PREFIX=%DJV_BUILD%/DJV-package-install
cmake --build . --config Release -j
cd DJV\src\DJV-build
cmake ..\..\..\..\..\DJV -DDJV_THIRD_PARTY=TRUE
cmake --build . --config Release -j
cmake --build . --config Release -j --target package
cd ..\..\..\..

python DJV\etc\Windows\package_map.py DJV-package\package.map
MakeAppx pack /f DJV-package\package.map /p DJV-package/DJV_2.0.9.0_x64.appx /o

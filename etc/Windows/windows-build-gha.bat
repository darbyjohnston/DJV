set BUILD_TYPE=%1

mkdir build
cd build
cmake ..\etc\SuperBuild ^
  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_PREFIX_PATH=install ^
  -DTLRENDER_FFMPEG=%TLRENDER_FFMPEG% ^
  -DTLRENDER_USD=%TLRENDER_USD% ^
  -Ddtk_API=%dtk_API%
cmake --build . -j 4 --config %BUILD_TYPE%

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Build Status](https://github.com/darbyjohnston/DJV/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/darbyjohnston/DJV/actions/workflows/ci-workflow.yml)

DJV
===
DJV is a media playback application for VFX, animation, and film production.

Features:
* Playback OpenTimelineIO timelines (.otio and .otioz), image sequences, and movies
* A/B comparison with multiple modes including wipe, overlay, difference, and tile
* Color management with OpenColorIO
* Multi-track, variable speed, and reverse audio playback
* Support for Linux, macOS, and Windows

Experimental:
* USD support

* [Download](https://darbyjohnston.github.io/DJV/index.html)
* [Documentation](https://darbyjohnston.github.io/DJV/index.html)

![Screenshot](etc/Images/djv_screenshot1.png)
![Screenshot](etc/Images/djv_screenshot2.png)

These screenshots show time images being compared, a render and a wireframe,
with a wipe and horizontal layout.


# Building on Linux

Clone the repository:
```
git clone https://github.com/darbyjohnston/DJV.git
```
Run CMake with the super build script:
```
cmake -S DJV/etc/SuperBuild -B Release -DCMAKE_INSTALL_PREFIX=$PWD/Release/install -DCMAKE_PREFIX_PATH=$PWD/Release/install -DCMAKE_BUILD_TYPE=Release
```
Start the build:
```
cmake --build Release -j 4 --config Release
```
Run the application:
```
export LD_LIBRARY_PATH=$PWD/Release/install/lib:$LD_LIBRARY_PATH
```
```
./Release/DJV/src/DJV-build/bin/djv/djv
```


# Building on macOS

Clone the repository:
```
git clone https://github.com/darbyjohnston/DJV.git
```
Run CMake with the super build script:
```
cmake -S DJV/etc/SuperBuild -B Release -DCMAKE_INSTALL_PREFIX=$PWD/Release/install -DCMAKE_PREFIX_PATH=$PWD/Release/install -DCMAKE_BUILD_TYPE=Release
```
Start the build:
```
cmake --build Release -j 4 --config Release
```
Run the application:
```
./Release/DJV/src/DJV-build/bin/djv/djv
```

## Notes for building on macOS

The CMake variable `CMAKE_OSX_ARCHITECTURES` can be used to specify the build
architecture:
```
-DCMAKE_OSX_ARCHITECTURES=x86_64
```
```
-DCMAKE_OSX_ARCHITECTURES=arm64
```

These aliases are convenient for switching between architectures:
```
alias arm="env /usr/bin/arch -arm64 /bin/zsh --login"
alias intel="env /usr/bin/arch -x86_64 /bin/zsh --login"
```


# Building on Windows

Dependencies:
* Install MSYS2 (https://www.msys2.org) for compiling FFmpeg.
* Install Strawberry Perl (https://strawberryperl.com/) for compiling network support.
* Install Python 3.11 for compiling USD.

Clone the repository:
```
git clone https://github.com/darbyjohnston/DJV.git
```
Run CMake with the super build script:
```
cmake -S DJV\etc\SuperBuild -B Release -DCMAKE_INSTALL_PREFIX=%CD%\Release\install -DCMAKE_PREFIX_PATH=%CD%\Release\install -DCMAKE_BUILD_TYPE=Release
```
Start the build:
```
cmake --build Release -j 4 --config Release
```
Run the application:
```
set PATH=%CD%\Release\install\bin;%PATH%
```
```
.\Release\DJV\src\DJV-build\bin\djv\Release\djv
```


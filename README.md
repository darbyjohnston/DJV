[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Build Status](https://dev.azure.com/darbyjohnston/DJV/_apis/build/repos/GitHub/badge?api-version=5.1-preview.2)]
[![Build Status](https://travis-ci.org/darbyjohnston/DJV.svg?branch=master)](https://travis-ci.org/darbyjohnston/DJV)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=darbyjohnston_DJV&metric=alert_status)](https://sonarcloud.io/dashboard?id=darbyjohnston_DJV)

DJV
===
DJV provides professional review software for VFX, animation, and film production.
Playback high resolution, high bit-depth, image sequences and videos, with frame
accurate control and color management. Available for Linux, Apple macOS, and
Microsoft Windows. Source code is provided under a BSD style open source license.

* [Download](https://darbyjohnston.github.io/DJV/download.html)
* [Contribute](https://darbyjohnston.github.io/DJV/contributing.html)
* [Documentation](https://darbyjohnston.github.io/DJV/documentation.html)


Building on Linux
-----------------

Required build tools:
* C++11 compiler
* [CMake 3.12](https://cmake.org)
* [git](https://git-scm.com)
* [NASM](https://www.nasm.us)

Required libraries:
* [OpenGL 4.1](https://www.opengl.org)

Clone the repository:

    > git clone https://github.com/darbyjohnston/DJV.git djv-git

Create a new sub-directory and start the build:

    > mkdir djv-git-Debug
    > cd djv-git-Debug
    > cmake ../djv-git -DCMAKE_BUILD_TYPE=Debug
    > make -j

Add the install path to your LD_LIBRARY_PATH: 

    > export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib

Run the confidence tests:

    > make test

Run the djv player application:

    > ./build/bin/djv

Additional information on building DJV can be found here:
* [Building on Linux](https://darbyjohnston.github.io/DJV/build_linux.html)
* [Building on OS X](https://darbyjohnston.github.io/DJV/build_osx.html)
* [Building on Windows](https://darbyjohnston.github.io/DJV/build_windows.html)


License
-------

DJV is released under a BSD style open source license, see this
[page](https://darbyjohnston.github.io/DJV/legal.html) for details.


Contributing
------------

See this [page](https://darbyjohnston.github.io/DJV/contributing.html) for information on contributing to DJV.


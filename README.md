[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Build Status](https://travis-ci.org/darbyjohnston/DJV.svg?branch=master)](https://travis-ci.org/darbyjohnston/DJV)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=darbyjohnston_DJV&metric=alert_status)](https://sonarcloud.io/dashboard?id=darbyjohnston_DJV)

DJV Imaging
===========
Professional media review software for VFX, animation, and film.

Features include:
* Real-time image sequence and movie playback
* Support for industry standard file formats including OpenEXR, DPX, and Cineon
* Available for Linux, OS X, and Windows

Links:
* [Download](http://djv.sourceforge.net/Download.html)
* [Screenshots](http://djv.sourceforge.net/Screenshots.html)
* [Documentation](http://djv.sourceforge.net/Documentation.html)
* [Forums](https://sourceforge.net/p/djv/discussion)


License
-------
DJV is released under a BSD style open source license, see this
[page](http://djv.sourceforge.net/Legal.html) for details.


Building on Linux
-----------------

Required build tools:
* C++11 compiler
* [CMake 3.12](https://cmake.org)
* [git](https://git-scm.com)
* [NASM](https://www.nasm.us)

Required libraries:
* [OpenGL 4.1](https://www.opengl.org)

First clone the repository:

    > git clone https://github.com/darbyjohnston/DJV.git djv-git

Then create a new sub-directory and start the build:

    > mkdir djv-git-Debug
    > cd djv-git-Debug
    > cmake ../djv-git -DCMAKE_BUILD_TYPE=Debug -DDJV_THIRD_PARTY=TRUE
    > make -j

After the build completes add the install path to your LD_LIBRARY_PATH: 

    > export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib

Run the confidence tests:

    > make test

Run the djv_view application:

    > ./build/bin/djv_view

Additional information on building DJV can be found here:
* [Building on Linux](http://djv.sourceforge.net/BuildLinux.html)
* [Building on OS X](http://djv.sourceforge.net/BuildOSX.html)
* [Building on Windows](http://djv.sourceforge.net/BuildWindows.html)

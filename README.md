DJV Imaging
===========
Professional media review software for VFX, animation, and film.

Features include:
* Real-time image sequence and movie playback
* Command line tools for batch processing
* Support for industry standard file formats including OpenEXR, DPX, and Cineon
* Available for Linux, OS X, and Windows
* Open source

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
#### Requirements

Build tools:
* C++11 compiler
* [CMake 3.12](https://cmake.org)
* [git](https://git-scm.com)

Libraries:
* [OpenGL 3.3](https://www.opengl.org)
* [GLM 0.9.9](https://glm.g-truc.net/0.9.9/index.html)
* [IlmBase 2.3](http://www.openexr.com/downloads.html)

Optional libraries:
* [libjpeg v9b](https://www.ijg.org)
* [libpng 1.6](http://www.libpng.org/pub/png/libpng.html)
* [libtiff 4.0](http://download.osgeo.org/libtiff)
* [OpenEXR 2.3](http://www.openexr.com/downloads.html)
* [FFmpeg 4.0](https://www.ffmpeg.org)

#### Building the Source Code

Clone the repository:

    > git clone https://github.com/darbyjohnston/DJV.git djv-git

Create a new sub-directory and start the build:

    > mkdir djv-git-Debug
    > cd djv-git-Debug
    > cmake ../djv-git -DCMAKE_BUILD_TYPE=Debug
    > make

After the build completes run the confidence tests:

    > make test

You can also start the djv_view application:

    > ./build/bin/djv_view

Additional information on building DJV can be found here:
* [Building on Linux](http://djv.sourceforge.net/BuildLinux.html)
* [Building on OS X](http://djv.sourceforge.net/BuildOSX.html)
* [Building on Windows](http://djv.sourceforge.net/BuildWindows.html)

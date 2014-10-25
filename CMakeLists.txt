cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0020 OLD)
cmake_policy(SET CMP0042 OLD)

#-------------------------------------------------------------------------------
# Building DJV
#
# The DJV software uses the CMake build system. The following steps describe
# how to setup third party software dependencies and build DJV from source.
# Note that these instructions are a work in progress.
#
#
# Building Third Party Dependencies
# ---------------------------------
#
# The DJV software has a number of third party dependencies that can either be
# installed on the system or built from scratch. By default the dependencies
# installed on the system will be used. You may override this behavior by
# building the dependencies as described below and using the
# CMAKE_PREFIX_PATH. The CMake modules that are used for finding the
# dependencies are located in "cmake/Modules".
#
# Linux:
#     
# * git clone git://git.code.sf.net/p/djv/git-third-party djv-git-third-party
# * cd djv-git-third-party
# * sh Linux-64.sh
# * cd ..
# * export CMAKE_PREFIX_PATH=/.../djv-git-third-party-Linux-64
#
# OS X:
#     
# * git clone git://git.code.sf.net/p/djv/git-third-party djv-git-third-party
# * cd djv-git-third-party
# * sh OSX-64.sh
# * fix libquicktime-1.2.4/config.h as described in the third party build
#   script and rebuild/install libquicktime
# * cd ..
# * export CMAKE_PREFIX_PATH=/.../djv-git-third-party-OSX-64
#
# Windows:
#
# * git clone git://git.code.sf.net/p/djv/git-third-party djv-git-third-party
# * cd djv-git-third-party
# * uncompress third party packges
# * apply patch/openexr.ImfCompressionAttibute.h.patch
# * cd ..
# * mkdir djv-git-third-party-Windows-64-cmake
# * cd djv-git-third-party-Windows-64-cmake
# * cmake -G "NMake Makefiles" ..\djv-git-third-party
# * nmake
# * nmake package
# * unzip package in directory of your choice
# * compile qtbase-opensource-src-5.3.2.zip and install into the unzipped
#   directory from the previous step; make sure to configure qt with desktop
#   OpenGL support: -opengl desktop; other usefull options are:
#   -opensource, -confirm-license, -nomake examples 
# * rezip package
# * set CMAKE_PREFIX_PATH=C:/.../djv-git-third-party-Windows-64
# * set PATH=%PATH%;C:\...\djv-git-third-party-Windows-64\bin
#
#
# Building DJV
# ------------
#
# Linux:
#
# * git clone git://git.code.sf.net/p/djv/git djv-git
# * mkdir djv-git-Linux-64
# * cd djv-git-Linux-64
# * cmake ../djv-git
# * make
# * make test
# * make package
#
# OS X:
#
# * git clone git://git.code.sf.net/p/djv/git djv-git
# * mkdir djv-git-OSX-64
# * cd djv-git-OSX-64
# * cmake ../djv-git
# * make
# * make test
# * make package
#
# Windows:
#
# * git clone git://git.code.sf.net/p/djv/git djv-git
# * mkdir djv-git-Windows-64
# * cd djv-git-Windows-64
# * cmake -G "NMake Makefiles" ..\djv-git
# * nmake
# * nmake test
# * nmake package
#
#
# Versioning Check List
# ---------------------
#
# Files to update:
#
# * Set the proper DJV version in this file
# * Update the DJV version numbers in doc/*.html
# * Update the version number in Doxyfile
#
# Git tag example:
#
# * git tag -a 1.0.4 -m "Version 1.0.4" df2f2e21232ceeab85ca40fc988aba5591fe9e64
# * git push origin 1.0.4
#
#
# Image I/O Check List
# --------------------
#
# New image I/O file extensions need to be added both to this file (in the
# packaging section), etc/Linux/djv_view.mime.xml, and etc/OSX/Info.plist.in.
#-------------------------------------------------------------------------------

# Set whether to include third party dependencies when packaging.

set(djvPackageThirdParty true)

#-------------------------------------------------------------------------------
# System Settings
#-------------------------------------------------------------------------------

if (APPLE)

    set(djvSystemName OSX)

    add_definitions(-DDJV_OSX)

    set(djvLibraryInstallDir lib)

elseif (UNIX)

    #string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} Linux _TMP)

    #if (_TMP)

        set(djvSystemName Linux)
        
        add_definitions(-DDJV_LINUX)

    #endif()

    set(djvLibraryInstallDir lib)

elseif (WIN32)

    set(djvSystemName Windows)

    add_definitions(-DDJV_WINDOWS)

    set(djvLibraryInstallDir bin)

endif (APPLE)

#message("djvSystemName = " ${djvSystemName})

#-------------------------------------------------------------------------------
# Build Settings
#-------------------------------------------------------------------------------

# Set the project version.

set(djvVersionMajor "1")
set(djvVersionMinor "0")
set(djvVersionPatch "4")

# Set the project name.

set(djvVersionName
    djv-${djvVersionMajor}.${djvVersionMinor}.${djvVersionPatch})

set(djvProjectName ${djvVersionName}-${djvSystemName})

project(${djvProjectName})

#message("djvVersionName = " ${djvVersionName})
#message("djvProjectName = " ${djvProjectName})

# Set the project architecture.

set(djvBuildArch 32)

if (CMAKE_SIZEOF_VOID_P EQUAL 8 OR CMAKE_CL_64)

	set(djvBuildArch 64)

endif()

#message("djvBuildArch = " ${djvBuildArch})

set(djvPackageName ${djvProjectName}-${djvBuildArch})

# CMake module path.

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

#message("CMAKE_MODULE_PATH = " ${CMAKE_MODULE_PATH})

# Set the output directories for the build.

set(djvBuildDir ${PROJECT_BINARY_DIR}/build)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${djvBuildDir}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${djvBuildDir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${djvBuildDir}/bin)

# This option will show the source code organized in folders when using an IDE.

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set the configuration file.

configure_file(
    ${PROJECT_SOURCE_DIR}/djvConfig.h.in
    ${PROJECT_BINARY_DIR}/djvConfig.h
    ${PROJECT_BINARY_DIR}/djvConfig.h
    @ONLY)

# Enable debug or release mode.

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# Enable shared libraries.

set(BUILD_SHARED_LIBS true)

# Enable memory-mapped file I/O support.

add_definitions(-DDJV_MMAP)

# Enable testing.

enable_testing()

# Enable gcov code coverage targets.

set(djvGcov false)

# Enable assertions.

add_definitions(-DDJV_ASSERT)

# Compiler options.

if (APPLE)

    if (djvBuildArch EQUAL 32)

        set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} "-arch i386")
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-arch i386")
        set(CMAKE_LDFLAGS ${CMAKE_LDFLAGS} "-arch i386")
    
    elseif (djvBuildArch EQUAL 64)

        set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} "-arch x86_64")
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-arch x86_64")
        set(CMAKE_LDFLAGS ${CMAKE_LDFLAGS} "-arch x86_64")
    
    endif (djvBuildArch EQUAL 32)

elseif (UNIX)

    if (CMAKE_COMPILER_IS_GNUCXX)

        # Disable warnings from XPM files.

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

        # Disable warnings from printf/sprintf.

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")

        # Profiling options.
    
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

        # Gcov options.
        
        if (djvGcov)

            INCLUDE(CodeCoverage)
            
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
    
        endif (djvGcov)
        
    endif (CMAKE_COMPILER_IS_GNUCXX)

endif (APPLE)

#message("CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})

#-------------------------------------------------------------------------------
# Third Party Dependencies
#-------------------------------------------------------------------------------

# Initialize the list of image I/O plugins used for dependencies.

set(djvImageIoPlugins ${djvImageIoPlugins} djvCineonPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvDpxPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvIffPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvIflPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvLutPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvPicPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvPpmPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvRlaPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvSgiPlugin)
set(djvImageIoPlugins ${djvImageIoPlugins} djvTargaPlugin)

# Threads

set(CMAKE_THREAD_PREFER_PTHREAD true)

find_package(Threads)

# zlib

if (WIN32)

    add_definitions(-DZLIB_DLL)

endif (WIN32)

find_package(ZLIB)

#message("ZLIB_FOUND = " ${ZLIB_FOUND})
#message("ZLIB_INCLUDE_DIRS = " ${ZLIB_INCLUDE_DIRS})
#message("ZLIB_LIBRARIES = " ${ZLIB_LIBRARIES})

if (ZLIB_FOUND)

    if (djvPackageThirdParty)
    
        if (WIN32)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/bin/zlib.dll
                DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.1.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.1.2.8.dylib
                DESTINATION lib)

        elseif (UNIX)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.so.1
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.so.1.2.8
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)

endif (ZLIB_FOUND)

# OpenGL

find_package(OpenGL REQUIRED)

# GLEW

find_package(GLEW REQUIRED)

#message("GLEW_INCLUDE_DIR = " ${GLEW_INCLUDE_DIR})
#message("GLEW_LIBRARIES = " ${GLEW_LIBRARIES})

if (djvPackageThirdParty)

    if (WIN32)
	
		install(
			FILES
			$ENV{CMAKE_PREFIX_PATH}/bin/GLEW.dll
			DESTINATION bin)

    elseif (APPLE)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libGLEW.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libGLEW.1.10.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libGLEW.1.10.0.dylib
            DESTINATION lib)

    elseif (UNIX)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libGLEW.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libGLEW.so.1.10
            $ENV{CMAKE_PREFIX_PATH}/lib/libGLEW.so.1.10.0
            DESTINATION lib)

    endif (WIN32)

endif (djvPackageThirdParty)

# IlmBase

if (WIN32)

    add_definitions(-DPLATFORM_WINDOWS)
    add_definitions(-DOPENEXR_DLL)

endif (WIN32)

find_package(IlmBase REQUIRED)

#message("ILMBASE_LIBRARIES = " ${ILMBASE_LIBRARIES})
#message("ILMBASE_INCLUDE_DIRS = " ${ILMBASE_INCLUDE_DIRS})
    
if (djvPackageThirdParty)

    if (WIN32)

		install(
			FILES
            $ENV{CMAKE_PREFIX_PATH}/bin/Half.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Iex.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/IlmThread.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Imath.dll
			DESTINATION bin)

    elseif (APPLE)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex-2_2.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath-2_2.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread-2_2.12.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath-2_2.12.dylib
            DESTINATION lib)

    elseif (UNIX)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libHalf.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libIex-2_2.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libIexMath-2_2.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libIlmThread-2_2.so.12.0.0
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath-2_2.so.12
            $ENV{CMAKE_PREFIX_PATH}/lib/libImath-2_2.so.12.0.0
            DESTINATION lib)

    endif (WIN32)

endif (djvPackageThirdParty)

# libjpeg

find_package(JPEG)

#message("JPEG_FOUND = " ${JPEG_FOUND})
#message("JPEG_INCLUDE_DIR = " ${JPEG_INCLUDE_DIR})
#message("JPEG_LIBRARIES = " ${JPEG_LIBRARIES})

if (JPEG_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvJpegPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			#install(
			#	FILES
			#	$ENV{CMAKE_PREFIX_PATH}/bin/jpeg.dll
			#	DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.9.dylib
                DESTINATION lib)
        
        elseif (UNIX)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.so.9
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.so.9.0.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)

endif (JPEG_FOUND)

# libpng

if (WIN32)

    add_definitions(-DPNG_USE_DLL)

endif (WIN32)

find_package(PNG)

#message("PNG_FOUND = " ${PNG_FOUND})
#message("PNG_INCLUDE_DIR = " ${PNG_INCLUDE_DIR})
#message("PNG_LIBRARIES = " ${PNG_LIBRARIES})

if (PNG_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvPngPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/png.dll
				DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.16.dylib
                DESTINATION lib)

        elseif (UNIX)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.so.16
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.so.16.8.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)
    
endif (PNG_FOUND)

# OpenEXR

find_package(OpenEXR)

#message("OPENEXR_FOUND = " ${OPENEXR_FOUND})
#message("OPENEXR_INCLUDE_DIR = " ${OPENEXR_INCLUDE_DIR})
#message("OPENEXR_LIBRARIES = " ${OPENEXR_LIBRARIES})

if (OPENEXR_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvOpenExrPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/IlmImf.dll
				DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf-2_2.22.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil-2_2.22.dylib
                DESTINATION lib)

        elseif (UNIX)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf-2_2.so.22
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf-2_2.so.22.0.0
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil-2_2.so.22
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil-2_2.so.22.0.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)
            
endif (OPENEXR_FOUND)

# libtiff

find_package(TIFF)

#message("TIFF_FOUND = " ${TIFF_FOUND})
#message("TIFF_INCLUDE_DIR = " ${TIFF_INCLUDE_DIR})
#message("TIFF_LIBRARIES = " ${TIFF_LIBRARIES})

if (TIFF_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvTiffPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/tiff.dll
				DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.5.dylib
                DESTINATION lib)

        elseif (UNIX)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.so.5.2.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)

endif (TIFF_FOUND)

# FFmpeg

if (WIN32)

else (WIN32)

    find_package(FFmpeg)

    #message("FFMPEG_FOUND = " ${FFMPEG_FOUND})
    #message("FFMPEG_INCLUDE_DIRS = " ${FFMPEG_INCLUDE_DIRS})
    #message("FFMPEG_LIBRARIES = " ${FFMPEG_LIBRARIES})

    if (FFMPEG_FOUND)
    
        if (djvPackageThirdParty)
        
            if (WIN32)

            elseif (APPLE)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.54.92.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.54.3.103.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.3.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.3.42.103.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.54.63.104.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.52.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.52.18.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.0.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.0.17.102.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.2.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.2.2.100.dylib
                    DESTINATION lib)
            
            elseif (UNIX)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.54.92.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.54.3.103
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.3
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.3.42.103
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.54.63.104
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.52
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.52.18.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.0
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.0.17.102
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.2
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.2.2.100
                    DESTINATION lib)

            endif (WIN32)

        endif (djvPackageThirdParty)

    endif (FFMPEG_FOUND)
    
endif (WIN32)

# libquicktime

if (WIN32)

else (WIN32)

    find_package(Libquicktime)

    #message("LIBQUICKTIME_FOUND = " ${LIBQUICKTIME_FOUND})
    #message("LIBQUICKTIME_INCLUDE_DIRS = " ${LIBQUICKTIME_INCLUDE_DIRS})
    #message("LIBQUICKTIME_LIBRARIES = " ${LIBQUICKTIME_LIBRARIES})

    if (LIBQUICKTIME_FOUND)

        set(djvImageIoPlugins ${djvImageIoPlugins} djvLibquicktimePlugin)

        if (djvPackageThirdParty)
        
            if (WIN32)

            elseif (APPLE)

                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.0.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libintl.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libintl.8.dylib
                    DESTINATION lib)

                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_audiocodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_ffmpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_mjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_rtjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_videocodec.so
                    DESTINATION lib/libquicktime)

            elseif (UNIX)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.so.0
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.so.0.0.0
                    DESTINATION lib)

                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_audiocodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_ffmpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_mjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_png.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_rtjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_videocodec.so
                    DESTINATION lib/libquicktime)

            endif (WIN32)

        endif (djvPackageThirdParty)
    
    endif (LIBQUICKTIME_FOUND)

endif (WIN32)

# QuickTime
#
# \todo QuickTime is only supported for 32-bit Windows builds.

if (WIN32)

    if (djvBuildArch EQUAL 32)

        find_package(QuickTime)

        #message("QUICKTIME_FOUND = " ${QUICKTIME_FOUND})
        #message("QUICKTIME_INCLUDE_DIRS = " ${QUICKTIME_INCLUDE_DIRS})
        #message("QUICKTIME_LIBRARIES = " ${QUICKTIME_LIBRARIES})
    
        if (QUICKTIME_FOUND)

            set(djvImageIoPlugins ${djvImageIoPlugins} djvQuickTimePlugin)

        endif (QUICKTIME_FOUND)

    endif (djvBuildArch EQUAL 32)

endif (WIN32)

# Qt

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

if (djvPackageThirdParty)

    if (WIN32)

		install(
			FILES
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Concurrent.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Core.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Gui.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Network.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5OpenGL.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5PrintSupport.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Sql.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Test.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Xml.dll
			#$ENV{CMAKE_PREFIX_PATH}/bin/icudt52.dll
			#$ENV{CMAKE_PREFIX_PATH}/bin/icuin52.dll
			#$ENV{CMAKE_PREFIX_PATH}/bin/icuuc52.dll
			DESTINATION bin)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/qtaccessiblewidgets.dll
            DESTINATION bin/accessible)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/qgenericbearer.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/qnativewifibearer.dll
            DESTINATION bin/bearer)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/qico.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/qgif.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/qjpeg.dll
            DESTINATION bin/imageformats)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/qminimal.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/qoffscreen.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll
            DESTINATION bin/platforms)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/printsupport/windowsprintersupport.dll
            DESTINATION bin/printsupport)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/qsqlite.dll
            DESTINATION bin/sqldrivers)

    elseif (APPLE)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.3.2.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.3.2.dylib
            DESTINATION lib)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/libqtaccessiblewidgets.dylib
            DESTINATION bin/accessible)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqcorewlanbearer.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqgenericbearer.dylib
            DESTINATION bin/bearer)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqico.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqgif.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqjpeg.dylib
            DESTINATION bin/imageformats)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqcocoa.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqminimal.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqoffscreen.dylib
            DESTINATION bin/platforms)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/printsupport/libcocoaprintersupport.dylib
            DESTINATION bin/printsupport)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlite.dylib
            DESTINATION bin/sqldrivers)

    elseif (UNIX)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5.3.2
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5.3.2
            DESTINATION lib)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqconnmanbearer.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqnmbearer.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqgenericbearer.so
            DESTINATION bin/bearer)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqico.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqgif.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqjpeg.so
            DESTINATION bin/imageformats)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/libqtaccessiblewidgets.so
            DESTINATION bin/accessible)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platformthemes/libqgtk2.so
            DESTINATION bin/platformthemes)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlpsql.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlite.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlmysql.so
            DESTINATION bin/sqldrivers)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
            DESTINATION bin/platforminputcontexts)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqlinuxfb.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqminimal.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqxcb.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqoffscreen.so
            DESTINATION bin/platforms)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevmouseplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevtouchplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevkeyboardplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevtabletplugin.so
            DESTINATION bin/generic)

    endif (WIN32)

endif (djvPackageThirdParty)

# Set the libraries used for linking to djvCore.

set(djvCoreLibs djvCore ${djvCoreDeps})

# Set the djvCore include directories.

set(djvCoreIncludeDirs
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${ILMBASE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR})

# Set the djvCore dependencies.

set(djvCoreDeps
    Qt5::Widgets
    Qt5::OpenGL
    ${ILMBASE_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS})

# Set the libraries used for linking to djvCore.

set(djvCoreLibs djvCore ${djvCoreDeps})

# Set the djvGui include directories.

set(djvGuiIncludeDirs
    ${CMAKE_SOURCE_DIR}/lib/djvGui
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${ILMBASE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR})

# Set the djvGui dependencies.

set(djvGuiDeps
    djvCore
    Qt5::Widgets
    ${ILMBASE_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS})

# Set the libraries used for linking to djvGui.

set(djvGuiLibs djvGui ${djvGuiDeps})

#-------------------------------------------------------------------------------
# Packaging
#-------------------------------------------------------------------------------

if (WIN32)

    # Set the type of package to build.

    set(CPACK_GENERATOR ZIP NSIS)
    
    # Add OS libraries to the package.
    
	set(CMAKE_INSTALL_DEBUG_LIBRARIES true)
	
	include(InstallRequiredSystemLibraries)

    # Set NSIS package options.
    	
    set(CPACK_NSIS_MUI_ICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv_view.ico)
    set(CPACK_NSIS_MUI_UNICON ${PROJECT_SOURCE_DIR}/etc/Windows/djv_view.ico)

    if (djvBuildArch EQUAL 32)
    
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    
    elseif (djvBuildArch EQUAL 64)

        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")

    endif (djvBuildArch EQUAL 32)
	
    # \todo How can we ask the user if they want these file associations?
    
	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	
		WriteRegStr HKCR '.cin' '' 'djv_view'

		WriteRegStr HKCR '.dpx' '' 'djv_view'

		WriteRegStr HKCR '.iff' '' 'djv_view'
		WriteRegStr HKCR '.z' '' 'djv_view'

		WriteRegStr HKCR '.ifl' '' 'djv_view'
		
		WriteRegStr HKCR '.jpeg' '' 'djv_view'
		WriteRegStr HKCR '.jpg' '' 'djv_view'
		WriteRegStr HKCR '.jfif' '' 'djv_view'

		WriteRegStr HKCR '.lut' '' 'djv_view'
		WriteRegStr HKCR '.1dl' '' 'djv_view'

		WriteRegStr HKCR '.exr' '' 'djv_view'

		WriteRegStr HKCR '.pic' '' 'djv_view'
		
		WriteRegStr HKCR '.png' '' 'djv_view'

		WriteRegStr HKCR '.ppm' '' 'djv_view'
		WriteRegStr HKCR '.pnm' '' 'djv_view'
		WriteRegStr HKCR '.pgm' '' 'djv_view'
		WriteRegStr HKCR '.pbm' '' 'djv_view'

		WriteRegStr HKCR '.qt' '' 'djv_view'
		WriteRegStr HKCR '.mov' '' 'djv_view'
		WriteRegStr HKCR '.mp4' '' 'djv_view'

		WriteRegStr HKCR '.rla' '' 'djv_view'
		WriteRegStr HKCR '.rpf' '' 'djv_view'

		WriteRegStr HKCR '.sgi' '' 'djv_view'
		WriteRegStr HKCR '.rgba' '' 'djv_view'
		WriteRegStr HKCR '.rgb' '' 'djv_view'
		WriteRegStr HKCR '.bw' '' 'djv_view'

		WriteRegStr HKCR '.tga' '' 'djv_view'
		
		WriteRegStr HKCR '.tiff' '' 'djv_view'
		WriteRegStr HKCR '.tif' '' 'djv_view'
		
		WriteRegStr HKCR 'djv_view' '' 'djv_view'
		WriteRegStr HKCR 'djv_view\\\\shell' '' 'open'
		WriteRegStr HKCR 'djv_view\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\bin\\\\djv_view.exe \\\"%1\\\"'
		")

	set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
 
		DeleteRegKey HKCR '.cin'

		DeleteRegKey HKCR '.dpx'

		DeleteRegKey HKCR '.iff'
		DeleteRegKey HKCR '.z'

		DeleteRegKey HKCR '.ifl'

		DeleteRegKey HKCR '.jpeg'
		DeleteRegKey HKCR '.jpg'
		DeleteRegKey HKCR '.jfif'
		
		DeleteRegKey HKCR '.lut'
		DeleteRegKey HKCR '.1dl'

		DeleteRegKey HKCR '.exr'

		DeleteRegKey HKCR '.pic'

		DeleteRegKey HKCR '.png'

		DeleteRegKey HKCR '.ppm'
		DeleteRegKey HKCR '.pnm'
		DeleteRegKey HKCR '.pgm'
		DeleteRegKey HKCR '.pbm'

		DeleteRegKey HKCR '.qt'
		DeleteRegKey HKCR '.mov'
		DeleteRegKey HKCR '.mp4'

		DeleteRegKey HKCR '.rla'
		DeleteRegKey HKCR '.rpf'

		DeleteRegKey HKCR '.sgi'
		DeleteRegKey HKCR '.rgba'
		DeleteRegKey HKCR '.rgb'
		DeleteRegKey HKCR '.bw'

		DeleteRegKey HKCR '.tga'

		DeleteRegKey HKCR '.tiff'
		DeleteRegKey HKCR '.tif'

        DeleteRegKey HKCR 'djv_view'
	    ")

    # Set the installation directory.
    
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${djvPackageName})
	
elseif (APPLE)

    # Set the type of package to build.
    
    set(CPACK_GENERATOR Bundle)

    # Add bundle files to the package.
    
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/startup.sh.in
        ${PROJECT_BINARY_DIR}/startup.sh)

    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/OSX/Info.plist.in
        ${PROJECT_BINARY_DIR}/Info.plist)
    
    # Set bundle options.
    
    set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv_view.icns)
    set(CPACK_BUNDLE_NAME ${djvPackageName})
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/etc/OSX/djv_view.icns)
    set(CPACK_BUNDLE_PLIST ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_BINARY_DIR}/startup.sh)

elseif (UNIX)

    # Set the type of package to build.

    set(CPACK_GENERATOR TGZ RPM DEB)

    # Set the installation directory.
    
    set(CPACK_SET_DESTDIR true)
    set(CPACK_INSTALL_PREFIX /usr/local/${djvPackageName})
    
    # Set RPM options.
    
    set(CPACK_RPM_PACKAGE_NAME djv)
    #set(CPACK_RPM_PACKAGE_RELOCATABLE true)
    
    # Set Debian options.
    
    set(CPACK_DEBIAN_PACKAGE_NAME djv)

endif (WIN32)

set(CPACK_PACKAGE_VERSION_MAJOR ${djvVersionMajor})
set(CPACK_PACKAGE_VERSION_MINOR ${djvVersionMinor})
set(CPACK_PACKAGE_VERSION_PATCH ${djvVersionPatch})
set(CPACK_PACKAGE_CONTACT http://djv.sourceforge.net)
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/README)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "DJV Imaging provides professional movie playback software for use in film, VFX, and computer animation.")

set(CPACK_PACKAGE_EXECUTABLES djv_view djv_view)

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)

set(CPACK_PACKAGE_FILE_NAME ${djvPackageName})

#message("CPACK_PACKAGE_FILE_NAME = " ${CPACK_PACKAGE_FILE_NAME})

#-------------------------------------------------------------------------------
# Sub Directories
#-------------------------------------------------------------------------------

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(tests)
add_subdirectory(doc)

# \todo CPack needs to come last?

include(CPack)


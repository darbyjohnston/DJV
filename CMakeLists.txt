# This is the top-level CMake file for building the DJV source code. Settings
# in this file generally do not need to be changed. For instructions on how to
# build the source code follow this link:
#
# doc/djvSourceCode.html

cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0020 OLD)

# Set the DJV version number.

set(djvVersionMajor "1")
set(djvVersionMinor "0")
set(djvVersionPatch "6")

set(djvVersion ${djvVersionMajor}.${djvVersionMinor}.${djvVersionPatch})

# Set the DJV project name.

set(djvProjectName djv-${djvVersion})

project(${djvProjectName})

# Find the build architecture.

set(djvBuildArch 32)

if (CMAKE_SIZEOF_VOID_P EQUAL 8 OR CMAKE_CL_64)

	set(djvBuildArch 64)

endif()

#message("djvBuildArch = " ${djvBuildArch})

# System specific settings.

if (APPLE)

    set(djvSystemName OSX)
    set(djvLibraryInstallDir lib)
    add_definitions(-DDJV_OSX)

elseif (UNIX)

    string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} Linux _TMP)
    if (_TMP)
        set(djvSystemName Linux)
        set(djvLibraryInstallDir lib)
        add_definitions(-DDJV_LINUX)
    endif()

elseif (WIN32)

    set(djvSystemName Windows)
    set(djvLibraryInstallDir bin)
    add_definitions(-DDJV_WINDOWS)

endif (APPLE)

#message("djvSystemName = " ${djvSystemName})

# Set the CMake module path.

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

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

# Set compiler specific options.

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

        # Set profiling options.
    
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

        # Set gcov options.
        
        if (djvGcov)

            include(CodeCoverage)
            
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
    
        endif (djvGcov)
        
    endif (CMAKE_COMPILER_IS_GNUCXX)

endif (APPLE)

# Set the list of image I/O plugins used as build dependencies.

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

# Setup third party dependencies.

set(djvPackageThirdParty false)

set(CMAKE_THREAD_PREFER_PTHREAD true)
find_package(Threads REQUIRED)

find_package(OpenGL REQUIRED)

include(djvThirdPartyGlew)
include(djvThirdPartyIlmBase)
include(djvThirdPartyQt)

include(djvThirdPartyZlib)
include(djvThirdPartyJpeg)
include(djvThirdPartyPng)
include(djvThirdPartyTiff)
include(djvThirdPartyOpenExr)
include(djvThirdPartyLibquicktime)
include(djvThirdPartyQuickTime)

# Set convenience variables for include directories and libraries.

set(djvCoreLibs djvCore ${djvCoreDeps})

set(djvCoreIncludeDirs
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${ILMBASE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR})

if (Qt5Core_DIR)

    set(djvCoreDeps
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::Core
        ${ILMBASE_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS})

else()

    set(djvCoreDeps
        Qt4::QtGui
        Qt4::QtOpenGL
        Qt4::QtCore
        ${ILMBASE_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS})

endif()

set(djvCoreLibs djvCore ${djvCoreDeps})

set(djvGuiIncludeDirs
    ${CMAKE_SOURCE_DIR}/lib/djvGui
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${ILMBASE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR})

if (Qt5Core_DIR)

    set(djvGuiDeps
        djvCore
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::Core
        ${ILMBASE_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS})

else()

    set(djvGuiDeps
        djvCore
        Qt4::QtGui
        Qt4::QtOpenGL
        Qt4::QtCore
        ${ILMBASE_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS})

endif()

set(djvGuiLibs djvGui ${djvGuiDeps})

# Setup packaging.

include(djvPackage)

# Go into sub directories.

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(tests)
add_subdirectory(doc)

include(CPack)


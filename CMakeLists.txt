cmake_minimum_required(VERSION 3.31)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0091 NEW)

set(DJV_VERSION_MAJOR "3")
set(DJV_VERSION_MINOR "0")
set(DJV_VERSION_PATCH "0")
set(DJV_VERSION ${DJV_VERSION_MAJOR}.${DJV_VERSION_MINOR}.${DJV_VERSION_PATCH})
add_definitions(-DDJV_VERSION_MAJOR=${DJV_VERSION_MAJOR})
add_definitions(-DDJV_VERSION_MINOR=${DJV_VERSION_MINOR})
add_definitions(-DDJV_VERSION_PATCH=${DJV_VERSION_PATCH})
add_definitions(-DDJV_VERSION="${DJV_VERSION}")

project(
    DJV
    VERSION ${DJV_VERSION}
    DESCRIPTION "Professional review software for visual effects, film, and animation"
    HOMEPAGE_URL "https://github.com/darbyjohnston/DJV"
    LANGUAGES CXX)

set(TLRENDER_FFMPEG TRUE CACHE BOOL "Set whether FFmpeg support is enabled")
set(TLRENDER_USD FALSE CACHE BOOL "Set whether USD support is enabled")

list(PREPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
elseif(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
    # \bug Should this be set automatically?
    set(CMAKE_BUILD_RPATH "@executable_path/../../../../../install/lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

find_package(tlRender REQUIRED)

include(djvPackage)
include(InstallRequiredSystemLibraries)

include_directories(${CMAKE_SOURCE_DIR}/lib)
add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(bin)
add_subdirectory(etc/Legal)

include(CPack)

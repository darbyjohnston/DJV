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
    DESCRIPTION "Professional review software for VFX, animation, and film production"
    HOMEPAGE_URL "https://github.com/darbyjohnston/DJV"
    LANGUAGES CXX)

list(PREPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

set(CMAKE_CXX_STANDARD 17)

find_package(tlRender REQUIRED)

set(TLRENDER_FFMPEG ON)
set(TLRENDER_USD ON)
include(djvPackage)

include_directories(${CMAKE_SOURCE_DIR}/lib)
add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(bin)

include(CPack)

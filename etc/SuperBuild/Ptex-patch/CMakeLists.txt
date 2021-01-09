cmake_minimum_required(VERSION 3.8.0)
project(Ptex)

option(PTEX_BUILD_STATIC_LIBS "Enable building Ptex static libraries" ON)
option(PTEX_BUILD_SHARED_LIBS "Enable building Ptex shared libraries" ON)
option(PRMAN_15_COMPATIBLE_PTEX "Enable PRMan 15 compatibility" OFF)

if (DEFINED ENV{CXXFLAGS_STD})
    if ($ENV{CXXFLAGS_STD} STREQUAL "c++17")
        set(CMAKE_CXX_STANDARD 17)
    elseif ($ENV{CXXFLAGS_STD} STREQUAL "c++14")
        set(CMAKE_CXX_STANDARD 14)
    elseif ($ENV{CXXFLAGS_STD} STREQUAL "c++11")
        set(CMAKE_CXX_STANDARD 11)
    else ()
        set(CMAKE_CXX_STANDARD 98)
    endif ()
else ()
    set(CMAKE_CXX_STANDARD 98)
endif ()
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_FIND_PACKAGE_RESOLVE_SYMLINKS ON)
set(CMAKE_INSTALL_MESSAGE LAZY) # Silence "Up-to-date:" install messages

if (NOT DEFINED PTEX_SHA)
    # Query git for current commit ID
    execute_process(
        COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE PTEX_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif ()

if (NOT DEFINED PTEX_VER)
    # Get the version string from a "version" file or from git
    if (EXISTS "${PROJECT_SOURCE_DIR}/version")
        file(STRINGS "${PROJECT_SOURCE_DIR}/version" PTEX_VER)
    else ()
        execute_process(
            COMMAND git describe --first-parent --always HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE PTEX_VER
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif ()
endif ()

# Transform PTEX_VER into PTEX_MAJOR_VERSION and PTEX_MINOR_VERSION
string(REPLACE "v" "" PTEX_VER_STRIPPED ${PTEX_VER})  # strip leading "v"
string(REPLACE "." ";" PTEX_VER_LIST ${PTEX_VER_STRIPPED})
list(LENGTH PTEX_VER_LIST PTEX_VER_LENGTH)
if (${PTEX_VER_LENGTH} LESS 2)
    message(FATAL_ERROR "Could not determine the Ptex library version")
endif ()

# The version variables are used to generate PtexVersion.h
list(GET PTEX_VER_LIST 0 PTEX_MAJOR_VERSION)
list(GET PTEX_VER_LIST 1 PTEX_MINOR_VERSION)

include(GNUInstallDirs)
include(CTest)
include(FindThreads)

find_package(ZLIB REQUIRED)

enable_testing()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    # Detect the build type from the $FLAVOR environment variable
    # Default to optimized Release builds when unspecified.
    if ("$ENV{FLAVOR}" MATCHES "debug")
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "type of build" FORCE)
    elseif ("$ENV{FLAVOR}" MATCHES "profile")
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "type of build" FORCE)
    else ()
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "type of build" FORCE)
    endif ()
endif ()

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif ()

if (MSVC)
    add_definitions(
        # forcing value to bool 'true' or 'false' (performance warning)
        /wd4800
        # conversion from 'double' to 'float', possible loss of data
        /wd4244
        /D_CRT_SECURE_NO_WARNINGS
    )
endif()

if (${PRMAN_15_COMPATIBLE_PTEX})
    add_definitions(-DPTEX_NO_LARGE_METADATA_BLOCKS)
endif ()

include_directories(src/ptex)

add_subdirectory(src/ptex)
add_subdirectory(src/utils)
add_subdirectory(src/tests)
add_subdirectory(src/doc)
add_subdirectory(src/build)

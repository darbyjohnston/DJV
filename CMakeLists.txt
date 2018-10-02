cmake_minimum_required(VERSION 3.0)

set(djvProjectName djv)
project(${djvProjectName})

#-------------------------------------------------------------------------------
# Version Number
#
# When changing version numbers the following files need to be updated:
#
# * Set the new version in the top-level CMakeLists.txt file
# * Set the new version in the top-level Doxyfile
# * Update the download links in doc/Download.html
# * Tag the git repository
#
# Git example for tagging a version:
#
# > git tag -a 1.2.0 -m "Version 1.2.0"
# > git push origin 1.2.0
#-------------------------------------------------------------------------------

set(djvVersionMajor "1")
set(djvVersionMinor "2")
set(djvVersionPatch "0")
set(djvVersion ${djvVersionMajor}.${djvVersionMinor}.${djvVersionPatch})

#-------------------------------------------------------------------------------
# Configuration
#-------------------------------------------------------------------------------

# Set the platform we are building for.
if(APPLE)
    set(djvSystemName OSX)
    add_definitions(-DDJV_OSX)
elseif(UNIX)
    string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} Linux _TMP)
    if(_TMP)
        set(djvSystemName Linux)
        add_definitions(-DDJV_LINUX)
        set(CMAKE_C_FLAGS -fPIC)
        set(CMAKE_CXX_FLAGS -fPIC)
    endif()
elseif(WIN32)
    set(djvSystemName Windows)
    add_definitions(-DDJV_WINDOWS)
endif()

# Update the CMake module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

# Set the output directories for the build.
set(djvBuildDir ${PROJECT_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${djvBuildDir}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${djvBuildDir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${djvBuildDir}/bin)

# Set miscellaneous options.
set(djvBuildArch x64)
add_definitions(-DDJV_MMAP)
add_definitions(-DDJV_ASSERT)
set(BUILD_SHARED_LIBS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set the configuration file.
configure_file(
    ${PROJECT_SOURCE_DIR}/djvConfig.h.in
    ${PROJECT_BINARY_DIR}/djvConfig.h
    @ONLY)

# Enable testing.
enable_testing()

# Enable gcov code coverage targets.
set(djvGcov false)

# Set compiler specific options.
if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX)
    
        # Disable warnings from XPM files.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
        
        # Disable warnings from printf/sprintf.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")

        # Set profiling options.    
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

        # Set gcov options.
        if(djvGcov)
            include(CodeCoverage)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
        endif()
    endif()
endif()

# Include directories.
include_directories(
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/bin
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_BINARY_DIR})

#-------------------------------------------------------------------------------
# Third Party Packages
#-------------------------------------------------------------------------------

set(djvThirdParty
    ZLIB
    GLM
    OpenAL
    IlmBase
    Qt)
set(djvThirdPartyPackage true)

find_package(OpenGL REQUIRED)

set(ZLIB_SHARED_LIBS OFF)
set(GLM_SHARED_LIBS ON)
set(OPENAL_SHARED_LIBS ON)
set(JPEG_SHARED_LIBS OFF)
set(TIFF_SHARED_LIBS OFF)
set(PNG_SHARED_LIBS OFF)
set(ILMBASE_SHARED_LIBS OFF)
set(OPENEXR_SHARED_LIBS OFF)
set(FFMPEG_SHARED_LIBS ON)
set(QT_SHARED_LIBS ON)

foreach(PACKAGE ${djvThirdParty})
    find_package(${PACKAGE} REQUIRED)
endforeach()

find_package(JPEG)
find_package(PNG)
find_package(TIFF)
find_package(OpenEXR)
find_package(FFmpeg)

set(djvThirdPartyOptional)
if(JPEG_FOUND)
    set(djvThirdPartyOptional ${djvThirdPartyOptional} JPEG)
endif()
if(PNG_FOUND)
    set(djvThirdPartyOptional ${djvThirdPartyOptional} PNG)
endif()
if(TIFF_FOUND)
    set(djvThirdPartyOptional ${djvThirdPartyOptional} TIFF)
endif()
if(OPENEXR_FOUND)
    set(djvThirdPartyOptional ${djvThirdPartyOptional} OpenEXR)
endif()
if(FFMPEG_FOUND)
    set(djvThirdPartyOptional ${djvThirdPartyOptional} FFMPEG)
endif()

#-------------------------------------------------------------------------------
# Sub-Directories
#-------------------------------------------------------------------------------

include(djvPackage)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(experiments)
add_subdirectory(doc)

include(CPack)


# Find the libTIFF library.
#
# This module defines the following variables:
#
# * TIFF_FOUND
# * TIFF_VERSION
# * TIFF_INCLUDE_DIRS
# * TIFF_LIBRARIES
#
# This module defines the following imported targets:
#
# * TIFF::tiff
#
# This module defines the following interfaces:
#
# * TIFF

set(TIFF_VERSION 4.1.0)

find_package(ZLIB)
find_package(JPEG)

find_path(TIFF_INCLUDE_DIR NAMES tiff.h)
set(TIFF_INCLUDE_DIRS
    ${TIFF_INCLUDE_DIR}
    ${JPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(TIFF_LIBRARY NAMES tiffd tiff)
else()
    find_library(TIFF_LIBRARY NAMES tiff)
endif()
set(TIFF_LIBRARIES
    ${TIFF_LIBRARY}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    TIFF
    REQUIRED_VARS TIFF_INCLUDE_DIR TIFF_LIBRARY)
mark_as_advanced(TIFF_INCLUDE_DIR TIFF_LIBRARY)

if(TIFF_FOUND AND NOT TARGET TIFF::tiff)
    add_library(TIFF::tiff UNKNOWN IMPORTED)
    set_target_properties(TIFF::tiff PROPERTIES
        IMPORTED_LOCATION "${TIFF_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS TIFF_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${TIFF_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "JPEG;ZLIB")
endif()
if(TIFF_FOUND AND NOT TARGET TIFF)
    add_library(TIFF INTERFACE)
    target_link_libraries(TIFF INTERFACE TIFF::tiff)
endif()

# Find the libTIFF library.
#
# This module defines the following variables:
#
# * TIFF_FOUND
#
# This module defines the following imported targets:
#
# * TIFF::TIFF
#
# This module defines the following interfaces:
#
# * TIFF

find_path(TIFF_INCLUDE_DIR NAMES tiff.h)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(TIFF_LIBRARY NAMES tiffd tiff)
else()
    find_library(TIFF_LIBRARY NAMES tiff)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    TIFF
    REQUIRED_VARS TIFF_INCLUDE_DIR TIFF_LIBRARY)
mark_as_advanced(TIFF_INCLUDE_DIR TIFF_LIBRARY)

if(TIFF_FOUND AND NOT TARGET TIFF::TIFF)
    add_library(TIFF::TIFF UNKNOWN IMPORTED)
    find_package(ZLIB)
    find_package(JPEG)
    set_target_properties(TIFF::TIFF PROPERTIES
        IMPORTED_LOCATION "${TIFF_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS TIFF_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${TIFF_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "JPEG;ZLIB")
endif()
if(TIFF_FOUND AND NOT TARGET TIFF)
    add_library(TIFF INTERFACE)
    target_link_libraries(TIFF INTERFACE TIFF::TIFF)
endif()


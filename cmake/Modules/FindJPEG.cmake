# Find the libjpeg library.
#
# This module defines the following variables:
#
# * JPEG_FOUND
# * JPEG_INCLUDE_DIRS
# * JPEG_LIBRARIES
#
# This module defines the following imported targets:
#
# * JPEG::jpeg
#
# This module defines the following interfaces:
#
# * JPEG

find_path(JPEG_INCLUDE_DIR NAMES jpeglib.h)
set(JPEG_INCLUDE_DIRS ${JPEG_INCLUDE_DIR})

find_library(JPEG_LIBRARY NAMES jpeg62 jpeg jpeg-static)
set(JPEG_LIBRARIES ${JPEG_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    JPEG
    REQUIRED_VARS JPEG_INCLUDE_DIR JPEG_LIBRARY)
mark_as_advanced(JPEG_INCLUDE_DIR JPEG_LIBRARY)

set(JPEG_COMPILE_DEFINITIONS JPEG_FOUND)
if(NOT JPEG_SHARED_LIBS)
    list(APPEND JPEG_COMPILE_DEFINITIONS JPEG_STATIC)
endif()

if(JPEG_FOUND AND NOT TARGET JPEG::jpeg)
    add_library(JPEG::jpeg UNKNOWN IMPORTED)
    set_target_properties(JPEG::jpeg PROPERTIES
        IMPORTED_LOCATION "${JPEG_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${JPEG_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIR}")
endif()
if(JPEG_FOUND AND NOT TARGET JPEG)
    add_library(JPEG INTERFACE)
    target_link_libraries(JPEG INTERFACE JPEG::jpeg)
endif()

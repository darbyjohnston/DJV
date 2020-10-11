# Find the libjpeg library.
#
# This module defines the following variables:
#
# * JPEG_FOUND
#
# This module defines the following imported targets:
#
# * JPEG::JPEG
#
# This module defines the following interfaces:
#
# * JPEG

find_path(JPEG_INCLUDE_DIR NAMES jpeglib.h)

find_library(JPEG_LIBRARY NAMES jpeg-static jpeg)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    JPEG
    REQUIRED_VARS JPEG_INCLUDE_DIR JPEG_LIBRARY)
mark_as_advanced(JPEG_INCLUDE_DIR JPEG_LIBRARY)

if(JPEG_FOUND AND NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG UNKNOWN IMPORTED)
    set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LOCATION "${JPEG_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS JPEG_FOUND)
    if(NOT JPEG_SHARED_LIBS)
		set_property(TARGET JPEG::JPEG APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS JPEG_STATIC)
    endif()
endif()
if(JPEG_FOUND AND NOT TARGET JPEG)
    add_library(JPEG INTERFACE)
    target_link_libraries(JPEG INTERFACE JPEG::JPEG)
endif()


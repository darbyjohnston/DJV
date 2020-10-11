# Find the OpenColorIO library.
#
# This module defines the following variables:
#
# * OCIO_FOUND
#
# This module defines the following imported targets:
#
# * OCIO::OCIO
#
# This module defines the following interfaces:
#
# * OCIO

find_path(OCIO_INCLUDE_DIR NAMES OpenColorIO/OpenColorIO.h)

find_library(OCIO_LIBRARY NAMES OpenColorIO PATH_SUFFIXES static)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OCIO
    REQUIRED_VARS OCIO_INCLUDE_DIR OCIO_LIBRARY)
mark_as_advanced(OCIO_INCLUDE_DIR OCIO_LIBRARY)

if(OCIO_FOUND AND NOT TARGET OCIO::OCIO)
    add_library(OCIO::OCIO UNKNOWN IMPORTED)
    set_target_properties(OCIO::OCIO PROPERTIES
        IMPORTED_LOCATION "${OCIO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OCIO_INCLUDE_DIR}")
    if(NOT OCIO_SHARED_LIBS)
		set_property(TARGET OCIO::OCIO APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS OpenColorIO_STATIC)
    endif()
endif()
if(OCIO_FOUND AND NOT TARGET OCIO)
    add_library(OCIO INTERFACE)
    target_link_libraries(OCIO INTERFACE OCIO::OCIO)
endif()


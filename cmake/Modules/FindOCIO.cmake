# Find the OpenColorIO library.
#
# This module defines the following variables:
#
# * OCIO_FOUND
# * OCIO_INCLUDE_DIRS
# * OCIO_LIBRARIES
# * OCIO_DEFINES
#
# This module defines the following imported targets:
#
# * OCIO::OCIO
#
# This module defines the following interfaces:
#
# * OCIO

find_path(OCIO_INCLUDE_DIR
    NAMES OpenColorIO.h
    PATH_SUFFIXES OpenColorIO)
set(OCIO_INCLUDE_DIRS
    ${OCIO_INCLUDE_DIR})

find_library(
	OCIO_LIBRARY NAMES OpenColorIO
	PATH_SUFFIXES static)
set(OCIO_LIBRARIES
    ${OCIO_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OCIO
    REQUIRED_VARS OCIO_INCLUDE_DIR OCIO_LIBRARY)
mark_as_advanced(OCIO_INCLUDE_DIR OCIO_LIBRARY)

set(OCIO_DEFINES)
if(NOT OCIO_SHARED_LIBS)
	set(OCIO_DEFINES OpenColorIO_STATIC)
endif()

if(OCIO_FOUND AND NOT TARGET OCIO::OCIO)
    add_library(OCIO::OCIO UNKNOWN IMPORTED)
    set_target_properties(OCIO::OCIO PROPERTIES
        IMPORTED_LOCATION "${OCIO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OCIO_INCLUDE_DIRS}"
		INTERFACE_COMPILE_DEFINITIONS ${OCIO_DEFINES})
endif()
if(OCIO_FOUND AND NOT TARGET OCIO)
    add_library(OCIO INTERFACE)
    target_link_libraries(OCIO INTERFACE OCIO::OCIO)
endif()

if(DJV_THIRD_PARTY AND OCIO_SHARED_LIBS)
    if(WIN32)
        # \todo
    elseif(APPLE)
		install(
			FILES
			${DJV_THIRD_PARTY}/lib/OpenColorIO.dylib
			DESTINATION ${DJV_INSTALL_LIB})
    else()
		 install(
			FILES
			${DJV_THIRD_PARTY}/lib/OpenColorIO.so
			DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()

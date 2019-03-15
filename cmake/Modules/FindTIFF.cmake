# Find the libTIFF library.
#
# This module defines the following variables:
#
# * TIFF_FOUND
# * TIFF_INCLUDE_DIRS
# * TIFF_LIBRARIES
#
# This module defines the following imported targets:
#
# * TIFF::TIFF
#
# This module defines the following interfaces:
#
# * TIFF

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)

find_path(TIFF_INCLUDE_DIR
    NAMES tiff.h)
set(TIFF_INCLUDE_DIRS
    ${TIFF_INCLUDE_DIR}
    ${JPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(TIFF_LIBRARY NAMES tiffd)
endif()
if(NOT TIFF_LIBRARY)
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

if(TIFF_FOUND AND NOT TARGET TIFF::TIFF)
    add_library(TIFF::TIFF UNKNOWN IMPORTED)
    set_target_properties(TIFF::TIFF PROPERTIES
        IMPORTED_LOCATION "${TIFF_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES ZLIB
        IMPORTED_LINK_INTERFACE_LIBRARIES JPEG
        INTERFACE_INCLUDE_DIRECTORIES "${TIFF_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS TIFF_FOUND)
endif()
if(TIFF_FOUND AND NOT TARGET TIFF)
    add_library(TIFF INTERFACE)
    target_link_libraries(TIFF INTERFACE TIFF::TIFF)
endif()

if(DJV_THIRD_PARTY AND TIFF_SHARED_LIBS)
    if(WIN32)
        # \todo
    elseif(APPLE)
        install(
            FILES
            ${TIFF_LIBRARY}
            ${DJV_THIRD_PARTY}/lib/libtiff.5.dylib
            ${DJV_THIRD_PARTY}/lib/libtiff.5.3.0.dylib
            DESTINATION ${DJV_INSTALL_LIB})
    else()
        install(
            FILES
            ${TIFF_LIBRARY}
            ${TIFF_LIBRARY}.5
            ${TIFF_LIBRARY}.5.3.0
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


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

set(TIFF_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(TIFF_INCLUDE_DIRS
    ${TIFF_INCLUDE_DIR}
    ${JPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(WIN32)
    set(TIFF_LIBRARY debug ${CMAKE_INSTALL_PREFIX}/lib/tiffd.lib optimized ${CMAKE_INSTALL_PREFIX}/lib/tiff.lib)
elseif(APPLE)
    if(TIFF_SHARED_LIBS)
        set(TIFF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libtiff.dylib)
    else()
        set(TIFF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libtiff.a)
    endif()
else()
    if(TIFF_SHARED_LIBS)
        set(TIFF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libtiff.so)
    else()
        set(TIFF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libtiff.a)
    endif()
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
        INTERFACE_INCLUDE_DIRECTORIES "${TIFF_INCLUDE_DIRS}")
endif()
if(TIFF_FOUND AND NOT TARGET TIFF)
    add_library(TIFF INTERFACE)
    target_link_libraries(TIFF INTERFACE TIFF::TIFF)
endif()


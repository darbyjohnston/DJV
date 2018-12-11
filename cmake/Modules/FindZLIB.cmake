# Find the zlib library.
#
# This module defines the following variables:
#
# * ZLIB_FOUND
# * ZLIB_INCLUDE_DIRS
# * ZLIB_LIBRARIES
#
# This module defines the following imported targets:
#
# * ZLIB::ZLIB
#
# This module defines the following interfaces:
#
# * ZLIB

find_path(ZLIB_INCLUDE_DIR NAMES zlib.h)
set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(ZLIB_LIBRARY NAMES z zlibd zlibstaticd)
else()
    find_library(ZLIB_LIBRARY NAMES z zlib zlibstatic)
endif()
set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ZLIB
    REQUIRED_VARS ZLIB_INCLUDE_DIR ZLIB_LIBRARY)
mark_as_advanced(ZLIB_INCLUDE_DIR ZLIB_LIBRARY)

if(ZLIB_FOUND AND NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB UNKNOWN IMPORTED)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        IMPORTED_LOCATION "${ZLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}")
endif()
if(ZLIB_FOUND AND NOT TARGET ZLIB)
    add_library(ZLIB INTERFACE)
    target_link_libraries(ZLIB INTERFACE ZLIB::ZLIB)
endif()

if(DJV_THIRD_PARTY)
    if(WIN32)
        install(
            FILES ${DJV_THIRD_PARTY}/bin/zlib.dll
            DESTINATION ${DJV_INSTALL_BIN})
    elseif(APPLE)
        install(
            FILES
            ${DJV_THIRD_PARTY}/lib/libz.dylib
            ${DJV_THIRD_PARTY}/lib/libz.1.dylib
            ${DJV_THIRD_PARTY}/lib/libz.1.2.11.dylib
            DESTINATION ${DJV_INSTALL_LIB})
    else()
        install(
            FILES
            ${DJV_THIRD_PARTY}/lib/libz.so
            ${DJV_THIRD_PARTY}/lib/libz.so.1
            ${DJV_THIRD_PARTY}/lib/libz.so.1.2.11
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


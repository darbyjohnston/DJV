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

set(ZLIB_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})

if(WIN32)
    if(ZLIB_SHARED_LIBS)
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/zlibd.lib)
        else()
            set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/zlib.lib)
        endif()
    else()
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/zlibstaticd.lib)
        else()
            set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/zlibstatic.lib)
        endif()
    endif()
elseif(APPLE)
    if(ZLIB_SHARED_LIBS)
        set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libz.dylib)
    else()
        set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libz.a)
    endif()
else()
    if(ZLIB_SHARED_LIBS)
        set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libz.so)
    else()
        set(ZLIB_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libz.a)
    endif()
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


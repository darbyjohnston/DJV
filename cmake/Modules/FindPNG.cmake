# Find the libpng library.
#
# This module defines the following variables:
#
# * PNG_FOUND
# * PNG_INCLUDE_DIRS
# * PNG_LIBRARIES
#
# This module defines the following imported targets:
#
# * PNG::PNG
#
# This module defines the following interfaces:
#
# * PNG

find_package(ZLIB REQUIRED)

set(PNG_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(PNG_INCLUDE_DIRS
    ${PNG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

if(WIN32)
    set(PNG_LIBRARY debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16_staticd.lib optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16_static.lib)
elseif(APPLE)
    if(PNG_SHARED_LIBS)
        set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng.dylib)
        set(PNG_LIBRARY_1 debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16d.dylib optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16.dylib)
        set(PNG_LIBRARY_2 debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16d.16.dylib optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16.16.dylib)
        set(PNG_LIBRARY_3 debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16d.16.34.0.dylib optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16.16.34.0.dylib)
    else()
        set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng.a)
    endif()
else()
    if(PNG_SHARED_LIBS)
        set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng.so)
        set(PNG_LIBRARY_1 debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16d.so optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16.so)
        set(PNG_LIBRARY_2 debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16d.so.16 optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16.so.16)
        set(PNG_LIBRARY_3 debug ${CMAKE_INSTALL_PREFIX}/lib/libpng16d.so.16.34.0 optimized ${CMAKE_INSTALL_PREFIX}/lib/libpng16.so.16.34.0)
    else()
        set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng.a)
    endif()
endif()
set(PNG_LIBRARIES
    ${PNG_LIBRARY}
    ${ZLIB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    PNG
    REQUIRED_VARS PNG_INCLUDE_DIR PNG_LIBRARY)
mark_as_advanced(PNG_INCLUDE_DIR PNG_LIBRARY)

if(PNG_FOUND AND NOT TARGET PNG::PNG)
    add_library(PNG::PNG UNKNOWN IMPORTED)
    set_target_properties(PNG::PNG PROPERTIES
        IMPORTED_LOCATION "${PNG_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES ZLIB 
        INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIRS}")
endif()
if(PNG_FOUND AND NOT TARGET PNG)
    add_library(PNG INTERFACE)
    target_link_libraries(PNG INTERFACE PNG::PNG)
endif()


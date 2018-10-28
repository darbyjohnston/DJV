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

find_path(PNG_INCLUDE_DIR
    NAMES png.h)
set(PNG_INCLUDE_DIRS
    ${PNG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(PNG_LIBRARY NAMES pngd png16d png16_staticd)
else()
    find_library(PNG_LIBRARY NAMES png png16 png16_static)
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
        INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS PNG_FOUND)
endif()
if(PNG_FOUND AND NOT TARGET PNG)
    add_library(PNG INTERFACE)
    target_link_libraries(PNG INTERFACE PNG::PNG)
endif()

if(DJV_THIRD_PARTY AND PNG_SHARED_LIBS)
    if(WIN32)
        # \todo
    elseif(APPLE)
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            install(
                FILES
                ${DJV_THIRD_PARTY}/lib/libpng.dylib
                ${DJV_THIRD_PARTY}/lib/libpng16d.dylib
                ${DJV_THIRD_PARTY}/lib/libpng16d.16.dylib
                ${DJV_THIRD_PARTY}/lib/libpng16d.16.34.0.dylib
                DESTINATION ${DJV_INSTALL_LIB})
        else()
            install(
                FILES
                ${DJV_THIRD_PARTY}/lib/libpng.dylib
                ${DJV_THIRD_PARTY}/lib/libpng16.dylib
                ${DJV_THIRD_PARTY}/lib/libpng16.16.dylib
                ${DJV_THIRD_PARTY}/lib/libpng16.16.34.0.dylib
                DESTINATION ${DJV_INSTALL_LIB})
        endif()
    else()
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            install(
                FILES
                ${DJV_THIRD_PARTY}/lib/libpng.so
                ${DJV_THIRD_PARTY}/lib/libpng16d.so
                ${DJV_THIRD_PARTY}/lib/libpng16d.so.16
                ${DJV_THIRD_PARTY}/lib/libpng16d.so.16.34.0
                DESTINATION ${DJV_INSTALL_LIB})
        else()
             install(
                FILES
                ${DJV_THIRD_PARTY}/lib/libpng.so
                ${DJV_THIRD_PARTY}/lib/libpng16.so
                ${DJV_THIRD_PARTY}/lib/libpng16.so.16
                ${DJV_THIRD_PARTY}/lib/libpng16.so.16.34.0
                DESTINATION ${DJV_INSTALL_LIB})
        endif()
    endif()
endif()


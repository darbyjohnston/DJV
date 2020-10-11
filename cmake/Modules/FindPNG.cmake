# Find the libpng library.
#
# This module defines the following variables:
#
# * PNG_FOUND
#
# This module defines the following imported targets:
#
# * PNG::PNG
#
# This module defines the following interfaces:
#
# * PNG

find_path(PNG_INCLUDE_DIR NAMES png.h)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(PNG_LIBRARY
        NAMES pngd png16d libpng16_staticd png png16 png16_static libpng16_static)
else()
    find_library(PNG_LIBRARY
        NAMES png png16 png16_static libpng16_static)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    PNG
    REQUIRED_VARS PNG_INCLUDE_DIR PNG_LIBRARY)
mark_as_advanced(PNG_INCLUDE_DIR PNG_LIBRARY)

if(PNG_FOUND AND NOT TARGET PNG::PNG)
    add_library(PNG::PNG UNKNOWN IMPORTED)
    find_package(ZLIB REQUIRED)
    set_target_properties(PNG::PNG PROPERTIES
        IMPORTED_LOCATION "${PNG_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS PNG_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES ZLIB)
endif()
if(PNG_FOUND AND NOT TARGET PNG)
    add_library(PNG INTERFACE)
    target_link_libraries(PNG INTERFACE PNG::PNG)
endif()


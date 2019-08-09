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
    find_library(PNG_LIBRARY NAMES pngd png16d libpng16_staticd png png16 png16_static libpng16_static)
else()
    find_library(PNG_LIBRARY NAMES png png16 png16_static libpng16_static)
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


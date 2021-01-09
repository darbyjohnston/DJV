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
# * PNG::png
#
# This module defines the following interfaces:
#
# * PNG

find_path(PNG_INCLUDE_DIR NAMES png.h)
set(PNG_INCLUDE_DIRS
    ${PNG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(PNG_LIBRARY
        NAMES png16d libpng16d libpng16_staticd pngd png16 png16_static libpng16_static png)
else()
    find_library(PNG_LIBRARY
        NAMES png16 png16_static libpng16_static png)
endif()
set(PNG_LIBRARIES
    ${PNG_LIBRARY}
    ${ZLIB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    PNG
    REQUIRED_VARS PNG_INCLUDE_DIR PNG_LIBRARY)
mark_as_advanced(PNG_INCLUDE_DIR PNG_LIBRARY)

if(PNG_FOUND AND NOT TARGET PNG::png)
    add_library(PNG::png UNKNOWN IMPORTED)
    find_package(ZLIB REQUIRED)
    set_target_properties(PNG::png PROPERTIES
        IMPORTED_LOCATION "${PNG_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS PNG_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES ZLIB)
endif()
if(PNG_FOUND AND NOT TARGET PNG)
    add_library(PNG INTERFACE)
    target_link_libraries(PNG INTERFACE PNG::png)
endif()

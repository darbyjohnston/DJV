# Find the Blosc library.
#
# This module defines the following variables:
#
# * Blosc_FOUND
# * Blosc_INCLUDE_DIRS
# * Blosc_LIBRARIES
#
# This module defines the following imported targets:
#
# * Blosc::blosc
#
# This module defines the following interfaces:
#
# * Blosc

find_path(Blosc_INCLUDE_DIR NAMES blosc.h)
set(Blosc_INCLUDE_DIRS
    ${Blosc_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

find_library(Blosc_LIBRARY NAMES blosc)
set(Blosc_LIBRARIES
    ${Blosc_LIBRARY}
    ${ZLIB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Blosc
    REQUIRED_VARS Blosc_INCLUDE_DIR Blosc_LIBRARY)
mark_as_advanced(Blosc_INCLUDE_DIR Blosc_LIBRARY)

if(Blosc_FOUND AND NOT TARGET Blosc::blosc)
    add_library(Blosc::blosc UNKNOWN IMPORTED)
    find_package(ZLIB REQUIRED)
    set_target_properties(Blosc::blosc PROPERTIES
        IMPORTED_LOCATION "${Blosc_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS Blosc_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${Blosc_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES ZLIB)
endif()
if(Blosc_FOUND AND NOT TARGET Blosc)
    add_library(Blosc INTERFACE)
    target_link_libraries(Blosc INTERFACE Blosc::blosc)
endif()

# Find the PicoJSON library.
#
# This module defines the following variables:
#
# * PicoJSON_FOUND
# * PicoJSON_INCLUDE_DIRS
#
# This module defines the following interfaces:
#
# * PicoJSON

find_path(PicoJSON_INCLUDE_DIR picojson/picojson.h)
#message("PicoJSON_INCLUDE_DIR="${PicoJSON_INCLUDE_DIR})
set(PicoJSON_INCLUDE_DIRS ${PicoJSON_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    PicoJSON
    REQUIRED_VARS PicoJSON_INCLUDE_DIR)
mark_as_advanced(PicoJSON_INCLUDE_DIR)

if(PicoJSON_FOUND AND NOT TARGET PicoJSON)
    add_library(PicoJSON INTERFACE)
    set_target_properties(PicoJSON PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PicoJSON_INCLUDE_DIRS}")
endif()

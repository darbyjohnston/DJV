# Find the RapidJSON library.
#
# This module defines the following variables:
#
# * RapidJSON_FOUND
# * RapidJSON_INCLUDE_DIRS
#
# This module defines the following interfaces:
#
# * RapidJSON

find_path(RapidJSON_INCLUDE_DIR rapidjson/rapidjson.h)
#message("RapidJSON_INCLUDE_DIR="${RapidJSON_INCLUDE_DIR})
set(RapidJSON_INCLUDE_DIRS ${RapidJSON_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    RapidJSON
    REQUIRED_VARS RapidJSON_INCLUDE_DIR)
mark_as_advanced(RapidJSON_INCLUDE_DIR)

if(RapidJSON_FOUND AND NOT TARGET RapidJSON)
    add_library(RapidJSON INTERFACE)
    set_target_properties(RapidJSON PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${RapidJSON_INCLUDE_DIRS}")
endif()

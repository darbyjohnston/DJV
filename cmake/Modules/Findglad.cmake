# Find the glad library.
#
# This module defines the following variables:
#
# * glad_FOUND
#
# This module defines the following imported targets:
#
# * glad::glad
#
# This module defines the following interfaces:
#
# * glad

find_path(glad_INCLUDE_DIR NAMES glad.h)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(glad_LIBRARY NAMES glad)
else()
    find_library(glad_LIBRARY NAMES glad)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    glad
    REQUIRED_VARS glad_INCLUDE_DIR glad_LIBRARY)
mark_as_advanced(glad_INCLUDE_DIR glad_LIBRARY)

if(glad_FOUND AND NOT TARGET glad::glad)
    add_library(glad::glad UNKNOWN IMPORTED)
    set_target_properties(glad::glad PROPERTIES
        IMPORTED_LOCATION "${glad_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${glad_INCLUDE_DIR}")
endif()
if(glad_FOUND AND NOT TARGET glad)
    add_library(glad INTERFACE)
    target_link_libraries(glad INTERFACE glad::glad)
endif()

# Find the OpenNURBS library.
#
# This module defines the following variables:
#
# * OpenNURBS_FOUND
# * OpenNURBS_INCLUDE_DIRS
# * OpenNURBS_LIBRARIES
#
# This module defines the following imported targets:
#
# * OpenNURBS::OpenNURBS
#
# This module defines the following interfaces:
#
# * OpenNURBS

find_path(OpenNURBS_INCLUDE_DIR NAMES opennurbs/opennurbs.h)
set(OpenNURBS_INCLUDE_DIRS ${OpenNURBS_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(OpenNURBS_LIBRARY NAMES opennurbs)
else()
    find_library(OpenNURBS_LIBRARY NAMES opennurbs)
endif()
set(OpenNURBS_LIBRARIES ${OpenNURBS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenNURBS
    REQUIRED_VARS OpenNURBS_INCLUDE_DIR OpenNURBS_LIBRARY)
mark_as_advanced(OpenNURBS_INCLUDE_DIR OpenNURBS_LIBRARY)

if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS::OpenNURBS)
    add_library(OpenNURBS::OpenNURBS UNKNOWN IMPORTED)
    set_target_properties(OpenNURBS::OpenNURBS PROPERTIES
        IMPORTED_LOCATION "${OpenNURBS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OpenNURBS_INCLUDE_DIR}")
endif()
if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS)
    add_library(OpenNURBS INTERFACE)
    target_link_libraries(OpenNURBS INTERFACE OpenNURBS::OpenNURBS)
endif()

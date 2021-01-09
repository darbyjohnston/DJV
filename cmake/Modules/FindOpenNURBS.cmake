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
# * OpenNURBS::opennurbs
#
# This module defines the following interfaces:
#
# * OpenNURBS

find_path(OpenNURBS_INCLUDE_DIR NAMES opennurbs/opennurbs.h)
set(OpenNURBS_INCLUDE_DIRS ${OpenNURBS_INCLUDE_DIR})

find_library(OpenNURBS_LIBRARY NAMES opennurbs)
set(OpenNURBS_LIBRARIES ${OpenNURBS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenNURBS
    REQUIRED_VARS OpenNURBS_INCLUDE_DIR OpenNURBS_LIBRARY)
mark_as_advanced(OpenNURBS_INCLUDE_DIR OpenNURBS_LIBRARY)

if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS::opennurbs)
    add_library(OpenNURBS::opennurbs UNKNOWN IMPORTED)
    set_target_properties(OpenNURBS::opennurbs PROPERTIES
        IMPORTED_LOCATION "${OpenNURBS_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS OpenNURBS_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${OpenNURBS_INCLUDE_DIR}")
    if(WIN32)
		set_property(TARGET OpenNURBS::opennurbs APPEND PROPERTY INTERFACE_LINK_LIBRARIES Shlwapi)
    endif()
endif()
if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS)
    add_library(OpenNURBS INTERFACE)
    target_link_libraries(OpenNURBS INTERFACE OpenNURBS::opennurbs)
endif()

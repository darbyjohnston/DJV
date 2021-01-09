# Find the rkcommon library.
#
# This module defines the following variables:
#
# * rkcommon_FOUND
# * rkcommon_INCLUDE_DIRS
# * rkcommon_LIBRARIES
#
# This module defines the following imported targets:
#
# * rkcommon::rkcommon
#
# This module defines the following interfaces:
#
# * rkcommon

find_package(TBB REQUIRED)

find_path(rkcommon_INCLUDE_DIR NAMES rkcommon/common.h)
set(rkcommon_INCLUDE_DIRS
    ${rkcommon_INCLUDE_DIR}
    ${TBB_INCLUDE_DIRS})

find_library(rkcommon_LIBRARY NAMES rkcommon)
set(rkcommon_LIBRARIES
    ${rkcommon_LIBRARY}
    ${TBB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    rkcommon
    REQUIRED_VARS rkcommon_INCLUDE_DIR rkcommon_LIBRARY)
mark_as_advanced(rkcommon_INCLUDE_DIR rkcommon_LIBRARY)

set(rkcommon_COMPILE_DEFINITIONS rkcommon_FOUND)
if(WIN32)
    list(APPEND rkcommon_COMPILE_DEFINITIONS NOMINMAX)
endif()

if(rkcommon_FOUND AND NOT TARGET rkcommon::rkcommon)
    add_library(rkcommon::rkcommon UNKNOWN IMPORTED)
    set_target_properties(rkcommon::rkcommon PROPERTIES
        IMPORTED_LOCATION "${rkcommon_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${rkcommon_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${rkcommon_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "TBB")
endif()
if(rkcommon_FOUND AND NOT TARGET rkcommon)
    add_library(rkcommon INTERFACE)
    target_link_libraries(rkcommon INTERFACE rkcommon::rkcommon)
endif()

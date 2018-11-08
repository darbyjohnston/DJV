# Find the dr_libs library.
#
# This module defines the following variables:
#
# * dr_libs_FOUND
# * dr_libs_INCLUDE_DIRS
#
# This module defines the following interfaces:
#
# * dr_libs

find_path(dr_libs_INCLUDE_DIR
  NAMES dr_wav.h
  PATH_SUFFIXES dr_libs)
set(dr_libs_INCLUDE_DIRS ${dr_libs_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    dr_libs
    REQUIRED_VARS dr_libs_INCLUDE_DIR)
mark_as_advanced(dr_libs_INCLUDE_DIR)

if(dr_libs_FOUND AND NOT TARGET dr_libs)
    add_library(dr_libs INTERFACE)
    set_target_properties(dr_libs PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${dr_libs_INCLUDE_DIRS}")
endif()

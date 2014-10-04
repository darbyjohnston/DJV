# Find the libquicktime library.
#
# This module defines the following variables:
#
# * LIBQUICKTIME_INCLUDE_DIRS - Include directories for libquicktime.
# * LIBQUICKTIME_LIBRARIES    - Libraries to link against libquicktime.
# * LIBQUICKTIME_FOUND        - True if libquicktime has been found and can be
#                               used.

find_path(LIBQUICKTIME_INCLUDE_DIR lqt/lqt.h)

set(LIBQUICKTIME_INCLUDE_DIRS ${LIBQUICKTIME_INCLUDE_DIR})

set(findLibraryOptions)

if (APPLE)

    # \todo The option NO_CMAKE_SYSTEM_PATH is used so that find_library() does
    # not find the Apple QuickTime library. This should only be an issue on
    # case-insensitive file systems like the default on OS X.

    set(findLibraryOptions NO_CMAKE_SYSTEM_PATH)

endif (APPLE)

find_library(
    LIBQUICKTIME_LIBRARY
    NAMES quicktime
    PATH_SUFFIXES lib64
    ${findLibraryOptions})

set(LIBQUICKTIME_LIBRARIES
    ${LIBQUICKTIME_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    LIBQUICKTIME
    REQUIRED_VARS
        LIBQUICKTIME_INCLUDE_DIR
        LIBQUICKTIME_LIBRARY)

mark_as_advanced(LIBQUICKTIME_INCLUDE_DIR LIBQUICKTIME_LIBRARY)

# Find the GLEW library.
#
# This module defines the following variables:
#
# * GLEW_INCLUDE_DIRS - Include directories for GLEW.
# * GLEW_LIBRARIES    - Libraries to link against GLEW.
# * GLEW_FOUND        - True if GLEW has been found and can be used.

find_path(GLEW_INCLUDE_DIR GL/glew.h)

set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})

find_library(
    GLEW_LIBRARY
    NAMES GLEW glew32
    PATH_SUFFIXES lib lib64)

set(GLEW_LIBRARIES
    ${GLEW_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    GLEW
    REQUIRED_VARS
        GLEW_INCLUDE_DIR
        GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)


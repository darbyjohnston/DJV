# Find the OpenEXR library.
#
# This module defines the following variables:
#
# * OPENEXR_INCLUDE_DIRS - Include directories for OpenEXR.
# * OPENEXR_LIBRARIES    - Libraries to link against OpenEXR.
# * OPENEXR_FOUND        - True if OpenEXR has been found and can be used.

find_path(
    OPENEXR_INCLUDE_DIR
    ImfRgba.h
	PATH_SUFFIXES OpenEXR)

set(OPENEXR_INCLUDE_DIRS ${OPENEXR_INCLUDE_DIR})

find_library(
    OPENEXR_LIBRARY
    NAMES IlmImf
    PATH_SUFFIXES lib64)

set(OPENEXR_LIBRARIES
    ${OPENEXR_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    OPENEXR
    REQUIRED_VARS
        OPENEXR_INCLUDE_DIR
        OPENEXR_LIBRARY)

mark_as_advanced(OPENEXR_INCLUDE_DIR OPENEXR_LIBRARY)

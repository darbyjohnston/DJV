# Find the IlmBase library.
#
# This module defines the following variables:
#
# * ILMBASE_INCLUDE_DIRS - Include directories for IlmBase.
# * ILMBASE_LIBRARIES    - Libraries to link against IlmBase.
# * ILMBASE_FOUND        - True if IlmBase has been found and can be used.

find_path(
    ILMBASE_INCLUDE_DIR
    half.h
	PATH_SUFFIXES OpenEXR)

set(ILMBASE_INCLUDE_DIRS ${ILMBASE_INCLUDE_DIR})

find_library(
    ILMBASE_IEX_LIBRARY
    NAMES Iex Iex-2_2
    PATH_SUFFIXES lib64)
find_library(
    ILMBASE_HALF_LIBRARY
    NAMES Half
    PATH_SUFFIXES lib64)
find_library(
    ILMBASE_IMATH_LIBRARY
    NAMES Imath Imath-2_2
    PATH_SUFFIXES lib64)
#find_library(
#    ILMBASE_IEX_MATH_LIBRARY
#    NAMES IexMath IexMath-2_2
#    PATH_SUFFIXES lib64)
find_library(
    ILMBASE_ILM_THREAD_LIBRARY
    NAMES IlmThread IlmThread-2_2
    PATH_SUFFIXES lib64)

set(ILMBASE_LIBRARIES
    ${ILMBASE_IEX_LIBRARY}
    ${ILMBASE_HALF_LIBRARY}
    ${ILMBASE_IMATH_LIBRARY}
#    ${ILMBASE_IEX_MATH_LIBRARY}
    ${ILMBASE_ILM_THREAD_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    ILMBASE
    REQUIRED_VARS
        ILMBASE_INCLUDE_DIR
        ILMBASE_IEX_LIBRARY
        ILMBASE_HALF_LIBRARY
        ILMBASE_IMATH_LIBRARY
#        ILMBASE_IEX_MATH_LIBRARY
        ILMBASE_ILM_THREAD_LIBRARY)

mark_as_advanced(ILMBASE_INCLUDE_DIR ILMBASE_LIBRARY)

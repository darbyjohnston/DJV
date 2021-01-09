# Find the Imath library.
#
# This module defines the following variables:
#
# * Imath_FOUND
# * Imath_INCLUDE_DIRS
# * Imath_LIBRARIES
#
# This module defines the following imported targets:
#
# * Imath::Imath
#
# This module defines the following interfaces:
#
# * Imath

find_path(Imath_INCLUDE_DIR NAMES half.h PATH_SUFFIXES Imath)
set(Imath_INCLUDE_DIRS ${Imath_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(Imath_LIBRARY NAMES Imath-3_0_d Imath-3_0)
else()
    find_library(Imath_LIBRARY NAMES Imath-3_0)
endif()
set(Imath_LIBRARIES ${Imath_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Imath
    REQUIRED_VARS Imath_INCLUDE_DIR Imath_LIBRARY)
mark_as_advanced(Imath_INCLUDE_DIR Imath_LIBRARY)

set(Imath_COMPILE_DEFINITIONS Imath_FOUND)
if(BUILD_SHARED_LIBS)
    list(APPEND Imath_COMPILE_DEFINITIONS IMATH_DLL)
endif()

if(Imath_FOUND AND NOT TARGET Imath::Imath)
    add_library(Imath::Imath UNKNOWN IMPORTED)
    set_target_properties(Imath::Imath PROPERTIES
        IMPORTED_LOCATION "${Imath_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${Imath_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${Imath_INCLUDE_DIR}")
endif()
if(Imath_FOUND AND NOT TARGET Imath)
    add_library(Imath INTERFACE)
    target_link_libraries(Imath INTERFACE Imath::Imath)
endif()

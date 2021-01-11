# Find the IlmBase library.
#
# This module defines the following variables:
#
# * IlmBase_FOUND
# * IlmBase_VERSION
# * IlmBase_INCLUDE_DIRS
# * IlmBase_LIBRARIES
#
# This module defines the following imported targets:
#
# * IlmBase::Half
# * IlmBase::Iex
# * IlmBase::IexMath
# * IlmBase::Imath
# * IlmBase::IlmThread
#
# This module defines the following interfaces:
#
# * IlmBase

set(IlmBase_VERSION 2.5)

find_path(IlmBase_INCLUDE_DIR NAMES half.h PATH_SUFFIXES OpenEXR)
set(Imath_INCLUDE_DIRS ${Imath_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(IlmBase_Half_LIBRARY NAMES Half-2_5_d Half-2_5)
    find_library(IlmBase_Iex_LIBRARY NAMES Iex-2_5_d Iex-2_5)
    find_library(IlmBase_IexMath_LIBRARY NAMES IexMath-2_5_d IexMath-2_5)
    find_library(IlmBase_Imath_LIBRARY NAMES Imath-2_5_d Imath-2_5)
    find_library(IlmBase_IlmThread_LIBRARY NAMES IlmThread-2_5_d IlmThread-2_5)
else()
    find_library(IlmBase_Half_LIBRARY NAMES Half-2_5)
    find_library(IlmBase_Iex_LIBRARY NAMES Iex-2_5)
    find_library(IlmBase_IexMath_LIBRARY NAMES IexMath-2_5)
    find_library(IlmBase_Imath_LIBRARY NAMES Imath-2_5)
    find_library(IlmBase_IlmThread_LIBRARY NAMES IlmThread-2_5)
endif()
set(IlmBase_LIBRARIES
    ${IlmBase_Half_LIBRARY}
    ${IlmBase_Iex_LIBRARY}
    ${IlmBase_IexMath_LIBRARY}
    ${IlmBase_Imath_LIBRARY}
    ${IlmBase_IlmThread_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    IlmBase
    REQUIRED_VARS 
        IlmBase_INCLUDE_DIR
        IlmBase_Half_LIBRARY
        IlmBase_Iex_LIBRARY
        IlmBase_IexMath_LIBRARY
        IlmBase_Imath_LIBRARY
        IlmBase_IlmThread_LIBRARY
    VERSION_VAR IlmBase_VERSION)
mark_as_advanced(
    IlmBase_INCLUDE_DIR
    IlmBase_Half_LIBRARY
    IlmBase_Iex_LIBRARY
    IlmBase_IexMath_LIBRARY
    IlmBase_Imath_LIBRARY
    IlmBase_IlmBase_LIBRARY)

set(IlmBase_COMPILE_DEFINITIONS IlmBase_FOUND)
if(BUILD_SHARED_LIBS)
    list(APPEND IlmBase_COMPILE_DEFINITIONS OPENEXR_DLL)
endif()

if(IlmBase_FOUND AND NOT TARGET IlmBase::Half)
    add_library(IlmBase::Half UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Half PROPERTIES
        IMPORTED_LOCATION "${IlmBase_Half_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${IlmBase_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${IlmBase_INCLUDE_DIR}")
endif()
if(IlmBase_FOUND AND NOT TARGET IlmBase::Iex)
    add_library(IlmBase::Iex UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Iex PROPERTIES
        IMPORTED_LOCATION "${IlmBase_Iex_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${IlmBase_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${IlmBase_INCLUDE_DIR}")
endif()
if(IlmBase_FOUND AND NOT TARGET IlmBase::IexMath)
    add_library(IlmBase::IexMath UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IexMath PROPERTIES
        IMPORTED_LOCATION "${IlmBase_IexMath_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${IlmBase_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${IlmBase_INCLUDE_DIR}")
endif()
if(IlmBase_FOUND AND NOT TARGET IlmBase::Imath)
    add_library(IlmBase::Imath UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Imath PROPERTIES
        IMPORTED_LOCATION "${IlmBase_Imath_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${IlmBase_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${IlmBase_INCLUDE_DIR}")
endif()
if(IlmBase_FOUND AND NOT TARGET IlmBase::IlmThread)
    add_library(IlmBase::IlmThread UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IlmThread PROPERTIES
        IMPORTED_LOCATION "${IlmBase_IlmThread_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${IlmBase_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${IlmBase_INCLUDE_DIR}")
endif()
if(IlmBase_FOUND AND NOT TARGET IlmBase)
    add_library(IlmBase INTERFACE)
    target_link_libraries(IlmBase INTERFACE IlmBase::Half)
    target_link_libraries(IlmBase INTERFACE IlmBase::Iex)
    target_link_libraries(IlmBase INTERFACE IlmBase::IexMath)
    target_link_libraries(IlmBase INTERFACE IlmBase::Imath)
    target_link_libraries(IlmBase INTERFACE IlmBase::IlmThread)
endif()

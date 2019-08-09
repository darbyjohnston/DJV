# Find the IlmBase library.
#
# This module defines the following variables:
#
# * ILMBASE_FOUND
# * ILMBASE_DEFINES
#
# This module defines the following imported targets:
#
# * IlmBase::Half
# * IlmBase::Iex
# * IlmBase::IexMath
# * IlmBase::IlmThread
# * IlmBase::Imath
# * IlmBase::Half_static
# * IlmBase::Iex_static
# * IlmBase::IexMath_static
# * IlmBase::IlmThread_static
# * IlmBase::Imath_static
#
# This module defines the following interfaces:
#
# * IlmBase
# * IlmBase_static

find_package(Threads REQUIRED)

find_path(ILMBASE_INCLUDE_DIR
    NAMES half.h
    PATH_SUFFIXES OpenEXR)

set(ILMBASE_DEFINES)

find_library(ILMBASE_HALF_STATIC_LIBRARY NAMES Half_s)
find_library(ILMBASE_IEX_STATIC_LIBRARY NAMES Iex_s)
find_library(ILMBASE_IEXMATH_STATIC_LIBRARY NAMES IexMath_s)
find_library(ILMBASE_ILMTHREAD_STATIC_LIBRARY NAMES IlmThread_s)
find_library(ILMBASE_IMATH_STATIC_LIBRARY NAMES Imath_s)
if(ILMBASE_SHARED_LIBS)
    find_library(ILMBASE_HALF_LIBRARY NAMES Half)
    find_library(ILMBASE_IEX_LIBRARY NAMES Iex)
    find_library(ILMBASE_IEXMATH_LIBRARY NAMES IexMath)
    find_library(ILMBASE_ILMTHREAD_LIBRARY NAMES IlmThread)
    find_library(ILMBASE_IMATH_LIBRARY NAMES Imath)
    if(WIN32)
        set(ILMBASE_DEFINES OPENEXR_DLL)
    endif()
else()
    set(ILMBASE_HALF_LIBRARY ${ILMBASE_HALF_STATIC_LIBRARY})
    set(ILMBASE_IEX_LIBRARY ${ILMBASE_IEX_STATIC_LIBRARY})
    set(ILMBASE_IEXMATH_LIBRARY ${ILMBASE_IEXMATH_STATIC_LIBRARY})
    set(ILMBASE_ILMTHREAD_LIBRARY ${ILMBASE_ILMTHREAD_STATIC_LIBRARY})
    set(ILMBASE_IMATH_LIBRARY ${ILMBASE_IMATH_STATIC_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ILMBASE
    REQUIRED_VARS
		ILMBASE_INCLUDE_DIR
		ILMBASE_HALF_LIBRARY
		ILMBASE_IEX_LIBRARY
		ILMBASE_IEXMATH_LIBRARY
		ILMBASE_ILMTHREAD_LIBRARY
		ILMBASE_IMATH_LIBRARY)
mark_as_advanced(
	ILMBASE_INCLUDE_DIR
	ILMBASE_HALF_LIBRARY
	ILMBASE_IEX_LIBRARY
	ILMBASE_IEXMATH_LIBRARY
	ILMBASE_ILMTHREAD_LIBRARY
	ILMBASE_IMATH_LIBRARY
	ILMBASE_HALF_STATIC_LIBRARY
	ILMBASE_IEX_STATIC_LIBRARY
	ILMBASE_IEXMATH_STATIC_LIBRARY
	ILMBASE_ILMTHREAD_STATIC_LIBRARY
	ILMBASE_IMATH_STATIC_LIBRARY
	ILMBASE_DEFINES)

if(ILMBASE_FOUND AND NOT TARGET IlmBase::Half)
    add_library(IlmBase::Half UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Half PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_HALF_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Half_static)
    add_library(IlmBase::Half_static UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Half_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_HALF_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Iex)
    add_library(IlmBase::Iex UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Iex PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEX_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Iex_static)
    add_library(IlmBase::Iex_static UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Iex_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEX_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IexMath)
    add_library(IlmBase::IexMath UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IexMath PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEXMATH_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IexMath_static)
    add_library(IlmBase::IexMath_static UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IexMath_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEXMATH_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IlmThread)
    add_library(IlmBase::IlmThread UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IlmThread PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_ILMTHREAD_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
    if(CMAKE_THREAD_LIBS_INIT)
        set_target_properties(IlmBase::IlmThread PROPERTIES
            INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IlmThread_static)
    add_library(IlmBase::IlmThread_static UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IlmThread_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_ILMTHREAD_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
    if(CMAKE_THREAD_LIBS_INIT)
        set_target_properties(IlmBase::IlmThread_static PROPERTIES
            INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Imath)
    add_library(IlmBase::Imath UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Imath PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IMATH_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Imath_static)
    add_library(IlmBase::Imath_static UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Imath_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IMATH_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase)
    add_library(IlmBase INTERFACE)
    target_link_libraries(IlmBase INTERFACE IlmBase::Half)
    target_link_libraries(IlmBase INTERFACE IlmBase::Iex)
    target_link_libraries(IlmBase INTERFACE IlmBase::IexMath)
    target_link_libraries(IlmBase INTERFACE IlmBase::IlmThread)
    target_link_libraries(IlmBase INTERFACE IlmBase::Imath)
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase_static)
    add_library(IlmBase_static INTERFACE)
    target_link_libraries(IlmBase_static INTERFACE IlmBase::Half_static)
    target_link_libraries(IlmBase_static INTERFACE IlmBase::Iex_static)
    target_link_libraries(IlmBase_static INTERFACE IlmBase::IexMath_static)
    target_link_libraries(IlmBase_static INTERFACE IlmBase::IlmThread_static)
    target_link_libraries(IlmBase_static INTERFACE IlmBase::Imath_static)
endif()


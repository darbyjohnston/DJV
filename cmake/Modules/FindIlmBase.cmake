# Find the IlmBase library.
#
# This module defines the following variables:
#
# * ILMBASE_FOUND
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

set(ILMBASE_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/OpenEXR)
set(ILMBASE_DEFINES)

if(WIN32)
    set(ILMBASE_HALF_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Half_s.lib)
    set(ILMBASE_IEX_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Iex_s.lib)
    set(ILMBASE_IEXMATH_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IexMath_s.lib)
    set(ILMBASE_ILMTHREAD_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IlmThread_s.lib)
    set(ILMBASE_IMATH_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Imath_s.lib)
    if(ILMBASE_SHARED_LIBS)
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Half.lib)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Iex.lib)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IexMath.lib)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IlmThread.lib)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Imath.lib)
        set(ILMBASE_DEFINES -DOPENEXR_DLL)
    else()
        set(ILMBASE_HALF_LIBRARY ${ILMBASE_HALF_STATIC_LIBRARY})
        set(ILMBASE_IEX_LIBRARY ${ILMBASE_IEX_STATIC_LIBRARY})
        set(ILMBASE_IEXMATH_LIBRARY ${ILMBASE_IEXMATH_STATIC_LIBRARY})
        set(ILMBASE_ILMTHREAD_LIBRARY ${ILMBASE_ILMTHREAD_STATIC_LIBRARY})
        set(ILMBASE_IMATH_LIBRARY ${ILMBASE_IMATH_STATIC_LIBRARY})
    endif()
elseif(APPLE)
    set(ILMBASE_HALF_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf_s.a)
    set(ILMBASE_IEX_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex_s.a)
    set(ILMBASE_IEXMATH_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath_s.a)
    set(ILMBASE_ILMTHREAD_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread_s.a)
    set(ILMBASE_IMATH_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath_s.a)
    if(ILMBASE_SHARED_LIBS)
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf.dylib)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex.dylib)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath.dylib)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread.dylib)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath.dylib)
    else()
        set(ILMBASE_HALF_LIBRARY ${ILMBASE_HALF_STATIC_LIBRARY})
        set(ILMBASE_IEX_LIBRARY ${ILMBASE_IEX_STATIC_LIBRARY})
        set(ILMBASE_IEXMATH_LIBRARY ${ILMBASE_IEXMATH_STATIC_LIBRARY})
        set(ILMBASE_ILMTHREAD_LIBRARY ${ILMBASE_ILMTHREAD_STATIC_LIBRARY})
        set(ILMBASE_IMATH_LIBRARY ${ILMBASE_IMATH_STATIC_LIBRARY})
    endif()
else()
    set(ILMBASE_HALF_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf_s.a)
    set(ILMBASE_IEX_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex_s.a)
    set(ILMBASE_IEXMATH_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath_s.a)
    set(ILMBASE_ILMTHREAD_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread_s.a)
    set(ILMBASE_IMATH_STATIC_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath_s.a)
    if(ILMBASE_SHARED_LIBS)
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf.so)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex.so)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath.so)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread.so)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath.so)
    else()
        set(ILMBASE_HALF_LIBRARY ${ILMBASE_HALF_STATIC_LIBRARY})
        set(ILMBASE_IEX_LIBRARY ${ILMBASE_IEX_STATIC_LIBRARY})
        set(ILMBASE_IEXMATH_LIBRARY ${ILMBASE_IEXMATH_STATIC_LIBRARY})
        set(ILMBASE_ILMTHREAD_LIBRARY ${ILMBASE_ILMTHREAD_STATIC_LIBRARY})
        set(ILMBASE_IMATH_LIBRARY ${ILMBASE_IMATH_STATIC_LIBRARY})
    endif()
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
            IMPORTED_LINK_DEPENDENT_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
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
            IMPORTED_LINK_DEPENDENT_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
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


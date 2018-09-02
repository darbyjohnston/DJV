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
#
# This module defines the following interfaces:
#
# * IlmBase

find_package(Threads REQUIRED)

set(ILMBASE_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/OpenEXR)
set(ILMBASE_DEFINES)

if(WIN32)
    set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Half.lib)
    set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Iex.lib)
    set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IexMath.lib)
    set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IlmThread.lib)
    set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Imath.lib)
    if(ILMBASE_SHARED_LIBS)
        set(ILMBASE_DEFINES -DOPENEXR_DLL)
    endif()
elseif(APPLE)
    if(ILMBASE_SHARED_LIBS)
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf.dylib)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex.dylib)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath.dylib)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread.dylib)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath.dylib)
    else()
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf.a)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex.a)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath.a)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread.a)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath.a)
    endif()
else()
    if(ILMBASE_SHARED_LIBS)
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf.so)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex.so)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath.so)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread.so)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath.so)
    else()
        set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libHalf.a)
        set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIex.a)
        set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIexMath.a)
        set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmThread.a)
        set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libImath.a)
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
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Iex)
    add_library(IlmBase::Iex UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Iex PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEX_LIBRARY}"
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
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IlmThread)
    add_library(IlmBase::IlmThread UNKNOWN IMPORTED)
    set_target_properties(IlmBase::IlmThread PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_ILMTHREAD_LIBRARY}"
        IMPORTED_LINK_DEPENDENT_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Imath)
    add_library(IlmBase::Imath UNKNOWN IMPORTED)
    set_target_properties(IlmBase::Imath PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IMATH_LIBRARY}"
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


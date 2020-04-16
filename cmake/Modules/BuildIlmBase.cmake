include(ExternalProject)

if(NOT IlmBaseThirdParty)
    set(ILMBASE_RUNTIME_DIR lib)
    if(WIN32)
        set(ILMBASE_RUNTIME_DIR bin)
    endif()
    set(IlmBase_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_STANDARD=11
        -DOPENEXR_VERSION_MAJOR=2
        -DOPENEXR_VERSION_MINOR=3
        -DOPENEXR_VERSION_PATCH=0
        -DOPENEXR_VERSION=2.3.0
        -DOPENEXR_SOVERSION=24
        -DOPENEXR_BUILD_SHARED=${ILMBASE_SHARED_LIBS}
        -DBUILD_SHARED_LIBS=${ILMBASE_SHARED_LIBS}
        -DRUNTIME_DIR=${ILMBASE_RUNTIME_DIR})
    if(WIN32 OR NOT ILMBASE_SHARED_LIBS)
        set(IlmBase_ARGS ${IlmBase_ARGS} -DBUILD_ILMBASE_STATIC=1)
    endif()
    ExternalProject_Add(
        IlmBaseThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/IlmBase
        URL http://github.com/openexr/openexr/releases/download/v2.3.0/ilmbase-2.3.0.tar.gz
        PATCH_COMMAND
            ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/ilmbase-patch/CMakeLists.txt
            ${CMAKE_CURRENT_BINARY_DIR}/IlmBase/src/IlmBaseThirdParty/CMakeLists.txt
        CMAKE_ARGS ${IlmBase_ARGS})
endif()

set(ILMBASE_FOUND TRUE)
set(ILMBASE_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/OpenEXR)
set(ILMBASE_HALF_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}Half_s${CMAKE_STATIC_LIBRARY_SUFFIX})
set(ILMBASE_IEX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}Iex_s${CMAKE_STATIC_LIBRARY_SUFFIX})
set(ILMBASE_IEXMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}IexMath_s${CMAKE_STATIC_LIBRARY_SUFFIX})
set(ILMBASE_ILMTHREAD_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}IlmThread_s${CMAKE_STATIC_LIBRARY_SUFFIX})
set(ILMBASE_IMATH_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}Imath_s${CMAKE_STATIC_LIBRARY_SUFFIX})

if(ILMBASE_FOUND AND NOT TARGET IlmBase::Half)
    add_library(IlmBase::Half UNKNOWN IMPORTED)
    add_dependencies(IlmBase::Half IlmBaseThirdParty)
    set_target_properties(IlmBase::Half PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_HALF_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Half_static)
    add_library(IlmBase::Half_static UNKNOWN IMPORTED)
    add_dependencies(IlmBase::Half_static IlmBaseThirdParty)
    set_target_properties(IlmBase::Half_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_HALF_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Iex)
    add_library(IlmBase::Iex UNKNOWN IMPORTED)
    add_dependencies(IlmBase::Iex IlmBaseThirdParty)
    set_target_properties(IlmBase::Iex PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEX_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Iex_static)
    add_library(IlmBase::Iex_static UNKNOWN IMPORTED)
    add_dependencies(IlmBase::Iex_static IlmBaseThirdParty)
    set_target_properties(IlmBase::Iex_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEX_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IexMath)
    add_library(IlmBase::IexMath UNKNOWN IMPORTED)
    add_dependencies(IlmBase::IexMath IlmBaseThirdParty)
    set_target_properties(IlmBase::IexMath PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEXMATH_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IexMath_static)
    add_library(IlmBase::IexMath_static UNKNOWN IMPORTED)
    add_dependencies(IlmBase::IexMath_static IlmBaseThirdParty)
    set_target_properties(IlmBase::IexMath_static PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IEXMATH_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::IlmThread)
    add_library(IlmBase::IlmThread UNKNOWN IMPORTED)
    add_dependencies(IlmBase::IlmThread IlmBaseThirdParty)
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
    add_dependencies(IlmBase::IlmThread_static IlmBaseThirdParty)
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
    add_dependencies(IlmBase::Imath IlmBaseThirdParty)
    set_target_properties(IlmBase::Imath PROPERTIES
        IMPORTED_LOCATION "${ILMBASE_IMATH_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ILMBASE_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${ILMBASE_DEFINES}")
endif()
if(ILMBASE_FOUND AND NOT TARGET IlmBase::Imath_static)
    add_library(IlmBase::Imath_static UNKNOWN IMPORTED)
    add_dependencies(IlmBase::Imath_static IlmBaseThirdParty)
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


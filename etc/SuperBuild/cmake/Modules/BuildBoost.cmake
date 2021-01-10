include(ExternalProject)

set(Boost_UPDATE_COMMAND ./bootstrap)
if(WIN32)
    set(Boost_UPDATE_COMMAND call ${Boost_UPDATE_COMMAND})
endif()

set(Boost_VARIANT release)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    set(Boost_VARIANT debug)
endif()

set(Boost_LIB_SUFFIX)
if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(Boost_LIB_SUFFIX -vc142-mt-gd-x64-1_74)
    else()
        set(Boost_LIB_SUFFIX -vc142-mt-x64-1_74)
    endif()
endif()

set(Boost_BUILD_COMMAND_EXTRA)
if(WIN32)
    set(Boost_DLLS
        ${CMAKE_INSTALL_PREFIX}/lib/boost_atomic${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_chrono${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_container${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_context${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_contract${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_coroutine${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_date_time${Boost_LIB_SUFFIX}.dll
        #${CMAKE_INSTALL_PREFIX}/lib/boost_exception${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_fiber${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_filesystem${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_graph${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_iostreams${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_locale${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_log_setup${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_log${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_math_c99f${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_math_c99l${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_math_c99${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_math_tr1f${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_math_tr1l${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_math_tr1${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_nowide${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_prg_exec_monitor${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_program_options${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_random${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_regex${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_serialization${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_stacktrace_noop${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_stacktrace_windbg${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_system${Boost_LIB_SUFFIX}.dll
        #${CMAKE_INSTALL_PREFIX}/lib/boost_test_exec_monitor${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_thread${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_timer${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_type_erasure${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_unit_test_framework${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_wave${Boost_LIB_SUFFIX}.dll
        ${CMAKE_INSTALL_PREFIX}/lib/boost_wserialization${Boost_LIB_SUFFIX}.dll)
    set(Boost_BUILD_COMMAND_EXTRA
        ${CMAKE_COMMAND}
            -E copy_if_different
            ${Boost_DLLS}
            ${CMAKE_INSTALL_PREFIX}/bin)
endif()
ExternalProject_Add(
    Boost
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Boost
    DEPENDS ZLIB
    URL https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.bz2
    UPDATE_COMMAND ${Boost_UPDATE_COMMAND}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ./b2
        install
        --prefix=${CMAKE_INSTALL_PREFIX}
        --without-python
        --without-mpi
        --disable-icu
        address-model=64
        variant=${Boost_VARIANT}
        link=shared
        threading=multi
        runtime-link=shared
    COMMAND ${Boost_BUILD_COMMAND_EXTRA}
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND "")

# CMake build (not supported by Boost yet):
#
#ExternalProject_Add(
#    Boost
#    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Boost
#    DEPENDS ZLIB
#    GIT_REPOSITORY https://github.com/boostorg/boost.git
#    GIT_TAG boost-1.74.0
#    CMAKE_ARGS
#        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
#        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
#        -DCMAKE_INSTALL_LIBDIR=lib
#        -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
#        -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
#        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
#        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})


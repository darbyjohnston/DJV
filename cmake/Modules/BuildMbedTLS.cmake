include(ExternalProject)

ExternalProject_Add(
    MbedTLSThirdParty
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/MbedTLS
    DEPENDS ZLIBThirdParty
    URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.16.2.tar.gz
    CMAKE_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DBUILD_SHARED_LIBS=${MbedTLS_SHARED_LIBS})
set(MbedTLS_FOUND TRUE)
set(MbedTLS_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(WIN32)
	set(MbedTLS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/mbedtls${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(MbedTLS_x509_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/mbedx509${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(MbedTLS_crypto_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(MbedTLS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libmbedtls${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(MbedTLS_x509_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libmbedx509${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(MbedTLS_crypto_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libmbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
set(MbedTLS_LIBRARIES ${MbedTLS_LIBRARY} ${MbedTLS_x509_LIBRARY} ${MbedTLS_crypto_LIBRARY})

if(MbedTLS_FOUND AND NOT TARGET MbedTLS::MbedTLS)
    add_library(MbedTLS::MbedTLS UNKNOWN IMPORTED)
    add_dependencies(MbedTLS::MbedTLS MbedTLSThirdParty)
    set_target_properties(MbedTLS::MbedTLS PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_FOUND)
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS::x509)
    add_library(MbedTLS::x509 UNKNOWN IMPORTED)
    add_dependencies(MbedTLS::x509 MbedTLSThirdParty)
    set_target_properties(MbedTLS::x509 PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_x509_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_x509_FOUND)
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS::crypto)
    add_library(MbedTLS::crypto UNKNOWN IMPORTED)
    add_dependencies(MbedTLS::crypto MbedTLSThirdParty)
    set_target_properties(MbedTLS::crypto PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_crypto_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_crypto_FOUND)
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS)
    add_library(MbedTLS INTERFACE)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::MbedTLS)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::x509)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::crypto)
endif()


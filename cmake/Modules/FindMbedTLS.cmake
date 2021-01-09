# Find the MbedTLS library.
#
# This module defines the following variables:
#
# * MbedTLS_FOUND
# * MbedTLS_INCLUDE_DIRS
# * MbedTLS_LIBRARIES
#
# This module defines the following imported targets:
#
# * MbedTLS::mbedtls
# * MbedTLS::mbedx509
# * MbedTLS::mbedcrypto
#
# This module defines the following interfaces:
#
# * MbedTLS

find_path(MbedTLS_INCLUDE_DIR NAMES mbedtls/net.h)
set(MbedTLS_INCLUDE_DIRS ${MbedTLS_INCLUDE_DIR})

find_library(MbedTLS_LIBRARY NAMES mbedtls)
find_library(MbedTLS_x509_LIBRARY NAMES mbedx509)
find_library(MbedTLS_crypto_LIBRARY NAMES mbedcrypto)
set(MbedTLS_LIBRARIES ${MbedTLS_LIBRARY} ${MbedTLS_x509_LIBRARY} ${MbedTLS_crypto_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    MbedTLS
    REQUIRED_VARS
        MbedTLS_INCLUDE_DIR
        MbedTLS_LIBRARY
        MbedTLS_x509_LIBRARY
        MbedTLS_crypto_LIBRARY)
mark_as_advanced(
    MbedTLS_INCLUDE_DIR
    MbedTLS_LIBRARY
    MbedTLS_x509_LIBRARY
    MbedTLS_crypto_LIBRARY)

if(MbedTLS_FOUND AND NOT TARGET MbedTLS::mbedtls)
    add_library(MbedTLS::mbedtls UNKNOWN IMPORTED)
    set_target_properties(MbedTLS::mbedtls PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIR}")
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS::mbedx509)
    add_library(MbedTLS::mbedx509 UNKNOWN IMPORTED)
    set_target_properties(MbedTLS::mbedx509 PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_x509_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_x509_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIR}")
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS::mbedcrypto)
    add_library(MbedTLS::mbedcrypto UNKNOWN IMPORTED)
    set_target_properties(MbedTLS::mbedcrypto PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_crypto_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_crypto_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIR}")
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS)
    add_library(MbedTLS INTERFACE)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::mbedtls)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::mbedx509)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::mbedcrypto)
endif()


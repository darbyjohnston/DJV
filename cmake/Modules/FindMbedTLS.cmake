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
# * MbedTLS::MbedTLS
#
# This module defines the following interfaces:
#
# * MbedTLS

find_path(MbedTLS_INCLUDE_DIR
    NAMES net.h
	PATH_SUFFIXES mbedtls)
set(MbedTLS_INCLUDE_DIRS ${MbedTLS_INCLUDE_DIR})

find_library(MbedTLS_LIBRARY NAMES mbedtls mbedx509 mbedcrypto)
set(MbedTLS_LIBRARIES ${MbedTLS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    MbedTLS
    REQUIRED_VARS MbedTLS_INCLUDE_DIR MbedTLS_LIBRARY)
mark_as_advanced(MbedTLS_INCLUDE_DIR MbedTLS_LIBRARY)

if(MbedTLS_FOUND AND NOT TARGET MbedTLS::MbedTLS)
    add_library(MbedTLS::MbedTLS UNKNOWN IMPORTED)
    set_target_properties(MbedTLS::MbedTLS PROPERTIES
        IMPORTED_LOCATION "${MbedTLS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MbedTLS_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS MbedTLS_FOUND)
endif()
if(MbedTLS_FOUND AND NOT TARGET MbedTLS)
    add_library(MbedTLS INTERFACE)
    target_link_libraries(MbedTLS INTERFACE MbedTLS::MbedTLS)
endif()

if(DJV_THIRD_PARTY AND MbedTLS_SHARED_LIBS)
    if(WIN32)
        install(
            FILES
            ${MbedTLS_LIBRARY}
            DESTINATION ${DJV_INSTALL_BIN})
    else()
        install(
            FILES
            ${MbedTLS_LIBRARY}
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


# Find the curl library.
#
# This module defines the following variables:
#
# * curl_FOUND
# * curl_INCLUDE_DIRS
# * curl_LIBRARIES
#
# This module defines the following imported targets:
#
# * curl::curl
#
# This module defines the following interfaces:
#
# * curl

find_package(MbedTLS REQUIRED)

find_path(curl_INCLUDE_DIR
    NAMES curl.h
	PATH_SUFFIXES curl)
set(curl_INCLUDE_DIRS
	${curl_INCLUDE_DIR}
    ${MbedTLS_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
	find_library(curl_LIBRARY NAMES libcurl-d libcurl)
else()
	find_library(curl_LIBRARY NAMES libcurl)
endif()
set(curl_LIBRARIES
    ${curl_LIBRARY}
    ${MbedTLS_LIBRARIES})

if(NOT curl_SHARED_LIBS)
    add_definitions(-DCURL_STATICLIB)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    curl
    REQUIRED_VARS curl_INCLUDE_DIR curl_LIBRARY)
mark_as_advanced(curl_INCLUDE_DIR curl_LIBRARY)

if(curl_FOUND AND NOT TARGET curl::curl)
    add_library(curl::curl UNKNOWN IMPORTED)
    set_target_properties(curl::curl PROPERTIES
        IMPORTED_LOCATION "${curl_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES MbedTLS
        INTERFACE_INCLUDE_DIRECTORIES "${curl_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS curl_FOUND)
endif()
if(curl_FOUND AND NOT TARGET curl)
    add_library(curl INTERFACE)
    target_link_libraries(curl INTERFACE curl::curl)
endif()

if(DJV_THIRD_PARTY AND curl_SHARED_LIBS)
    if(WIN32)
        install(
            FILES
            ${curl_LIBRARY}
            DESTINATION ${DJV_INSTALL_BIN})
    else()
        install(
            FILES
            ${curl_LIBRARY}
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


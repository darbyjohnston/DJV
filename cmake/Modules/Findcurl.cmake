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

if(WIN32)
elseif(APPLE)
else()
    find_package(MbedTLS REQUIRED)
endif()

find_path(curl_INCLUDE_DIR NAMES curl/curl.h)
set(curl_INCLUDE_DIRS ${curl_INCLUDE_DIR})
if(WIN32)
elseif(APPLE)
else()
    set(curl_INCLUDE_DIRS
        ${curl_INCLUDE_DIRS}
        ${MbedTLS_INCLUDE_DIRS})
endif()

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
	find_library(curl_LIBRARY NAMES curl-d libcurl-d curl libcurl libcurl_imp)
else()
	find_library(curl_LIBRARY NAMES curl libcurl libcurl_imp)
endif()
set(curl_LIBRARIES ${curl_LIBRARY})
if(WIN32)
elseif(APPLE)
else()
    set(curl_LIBRARIES
        ${curl_LIBRARIES}
        ${MbedTLS_LIBRARIES})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    curl
    REQUIRED_VARS curl_INCLUDE_DIR curl_LIBRARY)
mark_as_advanced(curl_INCLUDE_DIR curl_LIBRARY)

if(curl_FOUND AND NOT TARGET curl::curl)
    add_library(curl::curl UNKNOWN IMPORTED)
    set_target_properties(curl::curl PROPERTIES
        IMPORTED_LOCATION ${curl_LIBRARY}
	    INTERFACE_COMPILE_DEFINITIONS curl_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${curl_INCLUDE_DIR}")
	if(WIN32)
	    set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_LINK_LIBRARIES "winmm;ws2_32;advapi32;crypt32")
		if(NOT curl_SHARED_LIBS)
			set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)
		endif()
	elseif(APPLE)
        find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
        find_library(SECURITY Security REQUIRED)
	    set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${CORE_FOUNDATION};${SECURITY}")
	else()
	    set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${MbedTLS_INCLUDE_DIRS)
	    set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_LINK_LIBRARIES MbedTLS)
	endif()
endif()
if(curl_FOUND AND NOT TARGET curl)
    add_library(curl INTERFACE)
    target_link_libraries(curl INTERFACE curl::curl)
endif()


# Find the curl library.
#
# This module defines the following variables:
#
# * curl_FOUND
#
# This module defines the following imported targets:
#
# * curl::curl
#
# This module defines the following interfaces:
#
# * curl

find_path(curl_INCLUDE_DIR NAMES curl/curl.h)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
	find_library(curl_LIBRARY NAMES curl-d libcurl-d curl libcurl)
else()
	find_library(curl_LIBRARY NAMES curl libcurl)
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
        INTERFACE_INCLUDE_DIRECTORIES "${curl_INCLUDE_DIR};${MbedTLS_INCLUDE_DIRS}")
	if(WIN32)
	    set_property(TARGET curl::curl PROPERTY INTERFACE_LINK_LIBRARIES "winmm;ws2_32;advapi32;crypt32")
		if(NOT curl_SHARED_LIBS)
			set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)
		endif()
	elseif(APPLE)
        find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
        find_library(SECURITY Security REQUIRED)
	    set_property(TARGET curl::curl PROPERTY INTERFACE_LINK_LIBRARIES "${CORE_FOUNDATION};${SECURITY}")
	else()
	    set_property(TARGET curl::curl PROPERTY INTERFACE_LINK_LIBRARIES MbedTLS)
	endif()
endif()
if(curl_FOUND AND NOT TARGET curl)
    add_library(curl INTERFACE)
    target_link_libraries(curl INTERFACE curl::curl)
endif()


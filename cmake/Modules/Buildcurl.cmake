include(ExternalProject)

set(curl_ARGS
	-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
	-DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
	-DCMAKE_DEBUG_POSTFIX=
	-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
	-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
	-DBUILD_SHARED_LIBS=${CURL_SHARED_LIBS}
	-DCURL_DISABLE_LDAP=TRUE)
if(WIN32)
	set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_WINSSL=TRUE)
elseif (APPLE)
	set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_SECTRANSP=TRUE)
else()
	set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_MBEDTLS=TRUE)
	set(curl_DEPENDS ${curl_DEPENDS} MbedTLSThirdParty)
endif()
set(curl_DEPENDS ${curl_DEPENDS} ZLIBThirdParty)
ExternalProject_Add(
    curlThirdParty
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/curl
    DEPENDS ${curl_DEPENDS}
    URL https://github.com/curl/curl/releases/download/curl-7_65_1/curl-7.65.1.tar.bz2
    CMAKE_ARGS ${curl_ARGS})
    
set(curl_FOUND TRUE)
set(curl_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(WIN32)
	set(curl_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libcurl${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(curl_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libcurl${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
set(curl_LIBRARIES ${curl_LIBRARY})

if(curl_FOUND AND NOT TARGET curl::curl)
    add_library(curl::curl UNKNOWN IMPORTED)
    add_dependencies(curl::curl curlThirdParty)
	if(WIN32)
		set_property(TARGET curl::curl PROPERTY IMPORTED_LOCATION ${curl_LIBRARY})
		set_property(TARGET curl::curl PROPERTY INTERFACE_LINK_LIBRARIES winmm ws2_32 advapi32 crypt32)
		set_property(TARGET curl::curl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${curl_INCLUDE_DIRS})
		set_property(TARGET curl::curl PROPERTY INTERFACE_COMPILE_DEFINITIONS curl_FOUND)
		if(NOT curl_SHARED_LIBS)
			set_property(TARGET curl::curl APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)
		endif()
	elseif(APPLE)
		set_property(TARGET curl::curl PROPERTY IMPORTED_LOCATION ${curl_LIBRARY})
        find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
        find_library(SECURITY Security REQUIRED)
		set_property(TARGET curl::curl PROPERTY INTERFACE_LINK_LIBRARIES ${CORE_FOUNDATION} ${SECURITY})
		set_property(TARGET curl::curl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${curl_INCLUDE_DIRS})
		set_property(TARGET curl::curl PROPERTY INTERFACE_COMPILE_DEFINITIONS curl_FOUND)
	else()
		set_property(TARGET curl::curl PROPERTY IMPORTED_LOCATION ${curl_LIBRARY})
		set_property(TARGET curl::curl PROPERTY INTERFACE_LINK_LIBRARIES MbedTLS)
		set_property(TARGET curl::curl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${curl_INCLUDE_DIRS})
		set_property(TARGET curl::curl PROPERTY INTERFACE_COMPILE_DEFINITIONS curl_FOUND)
	endif()
endif()
if(curl_FOUND AND NOT TARGET curl)
    add_library(curl INTERFACE)
    target_link_libraries(curl INTERFACE curl::curl)
endif()


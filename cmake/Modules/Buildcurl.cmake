include(ExternalProject)

set(curl_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_DEBUG_POSTFIX=
    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
    -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=MultiThreaded$<$<CONFIG:Debug>:Debug>
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DBUILD_SHARED_LIBS=${CURL_SHARED_LIBS}
    -DCURL_DISABLE_LDAP=TRUE)
if(WIN32)
    set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_WINSSL=TRUE)
elseif (APPLE)
    #set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_SECTRANSP=TRUE)
    set(curl_ARGS ${curl_ARGS} -DCMAKE_INSTALL_LIBDIR=lib)
    set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_MBEDTLS=TRUE)
    set(curl_DEPENDS ${curl_DEPENDS} MbedTLS)
else()
    set(curl_ARGS ${curl_ARGS} -DCMAKE_INSTALL_LIBDIR=lib)
    set(curl_ARGS ${curl_ARGS} -DCMAKE_USE_MBEDTLS=TRUE)
    set(curl_DEPENDS ${curl_DEPENDS} MbedTLS)
endif()
set(curl_DEPENDS ${curl_DEPENDS} ZLIB)
ExternalProject_Add(
    curl
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/curl
    DEPENDS ${curl_DEPENDS}
    URL https://github.com/curl/curl/releases/download/curl-7_65_1/curl-7.65.1.tar.bz2
    CMAKE_ARGS ${curl_ARGS})


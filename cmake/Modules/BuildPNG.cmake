include(ExternalProject)

if(NOT PNGThirdParty)
    ExternalProject_Add(
        PNGThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PNG
        DEPENDS ZLIBThirdParty
        URL "http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.gz?download"
        CMAKE_ARGS
            -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=lib
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
            -DZLIB_SHARED_LIBS=${ZLIB_SHARED_LIBS}
            -DPNG_SHARED=${PNG_SHARED_LIBS})
endif()

set(PNG_FOUND TRUE)
set(PNG_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(WIN32)
	if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
		set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng16_staticd${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
		set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng16_static${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
else()
	if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
		set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng16d${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
		set(PNG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libpng16${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
endif()

set(PNG_LIBRARIES ${PNG_LIBRARY} ${ZLIB_LIBRARIES})

if(PNG_FOUND AND NOT TARGET PNG::PNG)
    add_library(PNG::PNG UNKNOWN IMPORTED)
    add_dependencies(PNG::PNG PNGThirdParty)
    set_target_properties(PNG::PNG PROPERTIES
        IMPORTED_LOCATION "${PNG_LIBRARY}"
        INTERFACE_LINK_LIBRARIES ZLIB 
        INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS PNG_FOUND)
endif()
if(PNG_FOUND AND NOT TARGET PNG)
    add_library(PNG INTERFACE)
    target_link_libraries(PNG INTERFACE PNG::PNG)
endif()


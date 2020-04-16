include(ExternalProject)

if(NOT FreeTypeThirdParty)
    ExternalProject_Add(
        FreeTypeThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/FreeType
        DEPENDS ZLIBThirdParty
        URL http://download.savannah.gnu.org/releases/freetype/freetype-2.10.0.tar.gz
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
            -DWITH_ZLIB=ON
            -DWITH_BZip2=OFF
            -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE
            -DWITH_PNG=OFF
            -DCMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE
            -DWITH_HarfBuzz=OFF
            -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE)
endif()

set(FreeType_FOUND TRUE)
set(FreeType_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/freetype2)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
	set(FreeType_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}freetyped${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(FreeType_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}freetype${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
set(FreeType_LIBRARIES ${FreeType_LIBRARY} ${ZLIB_LIBRARIES})

if(FreeType_FOUND AND NOT TARGET FreeType::FreeType)
    add_library(FreeType::FreeType UNKNOWN IMPORTED)
    add_dependencies(FreeType::FreeType FreeTypeThirdParty)
    set_target_properties(FreeType::FreeType PROPERTIES
        IMPORTED_LOCATION "${FreeType_LIBRARY}"
        INTERFACE_LINK_LIBRARIES ZLIB 
        INTERFACE_INCLUDE_DIRECTORIES "${FreeType_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FreeType_FOUND)
endif()
if(FreeType_FOUND AND NOT TARGET FreeType)
    add_library(FreeType INTERFACE)
    target_link_libraries(FreeType INTERFACE FreeType::FreeType)
endif()


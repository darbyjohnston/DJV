# Find the FreeType library.
#
# This module defines the following variables:
#
# * FreeType_FOUND
# * FreeType_INCLUDE_DIRS
# * FreeType_LIBRARIES
#
# This module defines the following imported targets:
#
# * FreeType::freetype
#
# This module defines the following interfaces:
#
# * FreeType

find_package(ZLIB REQUIRED)

find_path(FreeType_INCLUDE_DIR NAMES ft2build.h PATH_SUFFIXES freetype2)
set(FreeType_INCLUDE_DIRS
    ${FreeType_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(FreeType_LIBRARY NAMES freetyped freetype)
else()
    find_library(FreeType_LIBRARY NAMES freetype)
endif()
set(FreeType_LIBRARIES ${FreeType_LIBRARY} ${ZLIB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FreeType
    REQUIRED_VARS FreeType_INCLUDE_DIR FreeType_LIBRARY)
mark_as_advanced(FreeType_INCLUDE_DIR FreeType_LIBRARY)

if(FreeType_FOUND AND NOT TARGET FreeType::freetype)
    add_library(FreeType::freetype UNKNOWN IMPORTED)
    set_target_properties(FreeType::freetype PROPERTIES
        IMPORTED_LOCATION "${FreeType_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS FreeType_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${FreeType_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES ZLIB)
endif()
if(FreeType_FOUND AND NOT TARGET FreeType)
    add_library(FreeType INTERFACE)
    target_link_libraries(FreeType INTERFACE FreeType::freetype)
endif()

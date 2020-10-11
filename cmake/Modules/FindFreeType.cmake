# Find the FreeType library.
#
# This module defines the following variables:
#
# * FreeType_FOUND
#
# This module defines the following imported targets:
#
# * FreeType::FreeType
#
# This module defines the following interfaces:
#
# * FreeType

find_path(FreeType_INCLUDE_DIR NAMES ft2build.h PATH_SUFFIXES freetype2)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(FreeType_LIBRARY NAMES freetyped freetype)
else()
    find_library(FreeType_LIBRARY NAMES freetype)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FreeType
    REQUIRED_VARS FreeType_INCLUDE_DIR FreeType_LIBRARY)
mark_as_advanced(FreeType_INCLUDE_DIR FreeType_LIBRARY)

if(FreeType_FOUND AND NOT TARGET FreeType::FreeType)
    add_library(FreeType::FreeType UNKNOWN IMPORTED)
    find_package(ZLIB REQUIRED)
    set_target_properties(FreeType::FreeType PROPERTIES
        IMPORTED_LOCATION "${FreeType_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS FreeType_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${FreeType_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES ZLIB)
endif()
if(FreeType_FOUND AND NOT TARGET FreeType)
    add_library(FreeType INTERFACE)
    target_link_libraries(FreeType INTERFACE FreeType::FreeType)
endif()


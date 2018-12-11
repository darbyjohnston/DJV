# Find the glbinding library.
#
# This module defines the following variables:
#
# * glbinding_FOUND
# * glbinding_INCLUDE_DIRS
# * glbinding_LIBRARIES
#
# This module defines the following imported targets:
#
# * glbinding::glbinding
#
# This module defines the following interfaces:
#
# * glbinding

find_path(glbinding_INCLUDE_DIR
    NAMES glbinding/glbinding.h)
set(glbinding_INCLUDE_DIRS ${glbinding_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(glbinding_LIBRARY NAMES glbindingd glbinding)
else()
    find_library(glbinding_LIBRARY NAMES glbinding)
endif()
set(glbinding_LIBRARIES ${glbinding_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    glbinding
    REQUIRED_VARS glbinding_INCLUDE_DIR glbinding_LIBRARY)
mark_as_advanced(glbinding_INCLUDE_DIR glbinding_LIBRARY)

if(glbinding_FOUND AND NOT TARGET glbinding::glbinding)
    add_library(glbinding::glbinding UNKNOWN IMPORTED)
    set_target_properties(glbinding::glbinding PROPERTIES
        IMPORTED_LOCATION "${glbinding_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${glbinding_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS glbinding_FOUND)
endif()
if(glbinding_FOUND AND NOT TARGET glbinding)
    add_library(glbinding INTERFACE)
    target_link_libraries(glbinding INTERFACE glbinding::glbinding)
endif()

if(DJV_THIRD_PARTY AND glbinding_SHARED_LIBS)
    if(WIN32)
        install(
            FILES
            ${glbinding_LIBRARY}
            DESTINATION ${DJV_INSTALL_BIN})
    else()
        install(
            FILES
            ${glbinding_LIBRARY}
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


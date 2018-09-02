# Find the glbinding library.
#
# This module defines the following variables:
#
# * GLBINDING_FOUND
# * GLBINDING_INCLUDE_DIRS
# * GLBINDING_LIBRARIES
#
# This module defines the following imported targets:
#
# * glbinding::glbinding
#
# This module defines the following interfaces:
#
# * glbinding

set(GLBINDING_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(GLBINDING_INCLUDE_DIRS ${GLBINDING_INCLUDE_DIR})

if(WIN32)
    set(GLBINDING_LIBRARY optimized ${CMAKE_INSTALL_PREFIX}/lib/glbinding.lib debug ${CMAKE_INSTALL_PREFIX}/lib/glbindingd.lib)
elseif(APPLE)
    if(GLBINDING_SHARED_LIBS)
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.dylib)
    else()
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.a)
    endif()
else()
    if(GLBINDING_SHARED_LIBS)
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.so)
    else()
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.a)
    endif()
endif()
set(GLBINDING_LIBRARIES ${GLBINDING_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GLBINDING
    REQUIRED_VARS GLBINDING_INCLUDE_DIR GLBINDING_LIBRARY)
mark_as_advanced(GLBINDING_INCLUDE_DIR GLBINDING_LIBRARY)

if(GLBINDING_FOUND AND NOT TARGET glbinding::glbinding)
    add_library(glbinding::glbinding UNKNOWN IMPORTED)
    set_target_properties(glbinding::glbinding PROPERTIES
        IMPORTED_LOCATION "${GLBINDING_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLBINDING_INCLUDE_DIRS}")
endif()
if(GLBINDING_FOUND AND NOT TARGET glbinding)
    add_library(glbinding INTERFACE)
    target_link_libraries(glbinding INTERFACE glbinding::glbinding)
endif()


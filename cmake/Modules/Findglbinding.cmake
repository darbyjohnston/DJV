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
	if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/glbindingd.lib)
        set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/glbinding-auxd.lib)
    else()
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/glbinding.lib)
        set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/glbinding-aux.lib)
    endif()
elseif(APPLE)
    if(GLBINDING_SHARED_LIBS)
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbindingd.dylib)
            set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding_auxd.dylib)
        else()
            set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.dylib)
            set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding_aux.dylib)
        endif()
    else()
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbindingd.a)
            set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding-auxd.a)
        else()
            set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.a)
            set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding-aux.a)
        endif()
    endif()
else()
    if(GLBINDING_SHARED_LIBS)
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.so)
        set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding_aux.so)
    else()
        set(GLBINDING_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding.a)
        set(GLBINDING_AUX_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libglbinding_aux.a)
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
if(GLBINDING_FOUND AND NOT TARGET glbinding::glbinding_aux)
    add_library(glbinding::glbinding_aux UNKNOWN IMPORTED)
    set_target_properties(glbinding::glbinding_aux PROPERTIES
        IMPORTED_LOCATION "${GLBINDING_AUX_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLBINDING_INCLUDE_DIRS}")
endif()
if(GLBINDING_FOUND AND NOT TARGET glbinding)
    add_library(glbinding INTERFACE)
    target_link_libraries(glbinding INTERFACE glbinding::glbinding)
    target_link_libraries(glbinding INTERFACE glbinding::glbinding_aux)
endif()

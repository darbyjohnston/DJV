# Find the GLFW library.
#
# This module defines the following variables:
#
# * GLFW_FOUND
# * GLFW_INCLUDE_DIRS
# * GLFW_LIBRARIES
#
# This module defines the following imported targets:
#
# * GLFW::glfw
#
# This module defines the following interfaces:
#
# * GLFW

find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)
set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})

find_library(GLFW_LIBRARY NAMES glfw3dll glfw3 glfw)
set(GLFW_LIBRARIES ${GLFW_LIBRARY})
if(WIN32)
elseif(APPLE)
    find_library(CORE_VIDEO CoreVideo REQUIRED)
    find_library(IO_KIT IOKit REQUIRED)
    find_library(COCOA Cocoa REQUIRED)
    find_library(CARBON Carbon REQUIRED)
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${CORE_VIDEO} ${IO_KIT} ${COCOA} ${CARBON})
else()
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GLFW
    REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY)
mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

if(GLFW_FOUND AND NOT TARGET GLFW::glfw)
    add_library(GLFW::glfw UNKNOWN IMPORTED)
    set_target_properties(GLFW::glfw PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS GLFW_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}")
    if(WIN32)
    elseif(APPLE)
	    set_property(TARGET GLFW::glfw PROPERTY INTERFACE_LINK_LIBRARIES "${CORE_VIDEO};${IO_KIT};${COCOA};${CARBON}")
    else()
		set_property(TARGET GLFW::glfw APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
	    set_property(TARGET GLFW::glfw PROPERTY INTERFACE_LINK_LIBRARIES "${X11_LIBRARIES};${CMAKE_THREAD_LIBS_INIT};${CMAKE_DL_LIBS}")
    endif()
endif()
if(GLFW_FOUND AND NOT TARGET GLFW)
    add_library(GLFW INTERFACE)
    target_link_libraries(GLFW INTERFACE GLFW::glfw)
endif()

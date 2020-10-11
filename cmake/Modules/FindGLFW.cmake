# Find the GLFW library.
#
# This module defines the following variables:
#
# * GLFW_FOUND
#
# This module defines the following imported targets:
#
# * GLFW::GLFW
#
# This module defines the following interfaces:
#
# * GLFW

find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)

find_library(GLFW_LIBRARY NAMES glfw3 glfw)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GLFW
    REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY)
mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

if(GLFW_FOUND AND NOT TARGET GLFW::GLFW)
    add_library(GLFW::GLFW UNKNOWN IMPORTED)
    set_target_properties(GLFW::GLFW PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS GLFW_FOUND)
    if(DJV_PLATFORM_MACOS)
        find_library(CORE_VIDEO CoreVideo REQUIRED)
        find_library(IO_KIT IOKit REQUIRED)
        find_library(COCOA Cocoa REQUIRED)
        find_library(CARBON Carbon REQUIRED)
	    set_property(TARGET GLFW::GLFW PROPERTY INTERFACE_LINK_LIBRARIES "${CORE_VIDEO};${IO_KIT};${COCOA};${CARBON}")
    elseif(DJV_PLATFORM_LINUX)
        find_package(X11 REQUIRED)
		set_property(TARGET GLFW::GLFW APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
	    set_property(TARGET GLFW::GLFW PROPERTY INTERFACE_LINK_LIBRARIES "${X11_LIBRARIES};${CMAKE_DL_LIBS}")
    endif()
endif()
if(GLFW_FOUND AND NOT TARGET GLFW)
    add_library(GLFW INTERFACE)
    target_link_libraries(GLFW INTERFACE GLFW::GLFW)
endif()


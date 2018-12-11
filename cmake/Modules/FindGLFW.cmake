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
# * GLFW::GLFW
#
# This module defines the following interfaces:
#
# * GLFW

find_path(GLFW_INCLUDE_DIR
    NAMES GLFW/glfw3.h)
set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})

find_library(GLFW_LIBRARY NAMES glfw3)
set(GLFW_LIBRARIES ${GLFW_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GLFW
    REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY)
mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

if(GLFW_FOUND AND NOT TARGET GLFW::GLFW)
    add_library(GLFW::GLFW UNKNOWN IMPORTED)
    set_target_properties(GLFW::GLFW PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS GLFW_FOUND)
endif()
if(GLFW_FOUND AND NOT TARGET GLFW)
    add_library(GLFW INTERFACE)
    target_link_libraries(GLFW INTERFACE GLFW::GLFW)
endif()

if(DJV_THIRD_PARTY AND GLFW_SHARED_LIBS)
    if(WIN32)
        install(
            FILES
            ${GLFW_LIBRARY}
            DESTINATION ${DJV_INSTALL_BIN})
    else()
        install(
            FILES
            ${GLFW_LIBRARY}
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


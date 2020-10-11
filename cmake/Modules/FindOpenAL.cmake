# Find the OpenAL library.
#
# This module defines the following variables:
#
# * OPENAL_FOUND
#
# This module defines the following imported targets:
#
# * OpenAL::OpenAL
#
# This module defines the following interfaces:
#
# * OpenAL

find_path(OPENAL_INCLUDE_DIR NAMES AL/al.h)

find_library(OPENAL_LIBRARY NAMES openal OpenAL32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OPENAL
    REQUIRED_VARS OPENAL_INCLUDE_DIR OPENAL_LIBRARY)
mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)

if(OPENAL_FOUND AND NOT TARGET OpenAL::OpenAL)
    add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
    set_target_properties(OpenAL::OpenAL PROPERTIES
        IMPORTED_LOCATION "${OPENAL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}")
    if(NOT OPENAL_SHARED_LIBS)
		set_property(TARGET OpenAL::OpenAL APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS AL_LIBTYPE_STATIC)
    endif()
    if(WIN32)
		set_property(TARGET OpenAL::OpenAL PROPERTY INTERFACE_LINK_LIBRARIES "Winmm")
    elseif(APPLE)
        find_library(AUDIO_TOOLBOX AudioToolbox)
        find_library(CORE_AUDIO CoreAudio)
		set_property(TARGET OpenAL::OpenAL PROPERTY INTERFACE_LINK_LIBRARIES "${AUDIO_TOOLBOX};${CORE_AUDIO}")
    endif()
endif()
if(OPENAL_FOUND AND NOT TARGET OpenAL)
    add_library(OpenAL INTERFACE)
    target_link_libraries(OpenAL INTERFACE OpenAL::OpenAL)
endif()

if(OPENAL_FOUND)
    if(WIN32)
        install(
            FILES
            ${CMAKE_PREFIX_PATH}/bin/OpenAL32.dll
            DESTINATION bin)
    elseif(APPLE)
        install(
            FILES
            ${OPENAL_LIBRARY}
            ${CMAKE_PREFIX_PATH}/lib/libopenal.1.dylib
            ${CMAKE_PREFIX_PATH}/lib/libopenal.1.19.1.dylib
            DESTINATION lib)
    else()
        install(
            FILES
            ${OPENAL_LIBRARY}
            ${OPENAL_LIBRARY}.1
            ${OPENAL_LIBRARY}.1.19.1
            DESTINATION lib)
    endif()
endif()


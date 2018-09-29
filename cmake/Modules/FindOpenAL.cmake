# Find the OpenAL library.
#
# This module defines the following variables:
#
# * OPENAL_FOUND 
# * OPENAL_INCLUDE_DIRS
# * OPENAL_LIBRARIES
# * OPENAL_DEFINES
#
# This module defines the following imported targets:
#
# * OpenAL::OpenAL
#
# This module defines the following interfaces:
#
# * OpenAL

set(OPENAL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
set(OPENAL_DEFINES)

if(WIN32)
    set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/OpenAL32.lib)
    set(OPENAL_LIBRARIES ${OPENAL_LIBRARY} Winmm)
    if(djvThirdPartyPackage)
        install(
            FILES
            ${CMAKE_INSTALL_PREFIX}/bin/OpenAL32.dll
            DESTINATION bin)
    endif()
elseif(APPLE)
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(CORE_AUDIO CoreAudio)
    if(OPENAL_SHARED_LIBS)
        set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal.dylib)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${OPENAL_LIBRARY}
                ${CMAKE_INSTALL_PREFIX}/lib/libopenal.1.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libopenal.1.18.2.dylib
                DESTINATION lib)
        endif()
    else()
        set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal.a)
    endif()
    set(OPENAL_LIBRARIES
        ${OPENAL_LIBRARY}
        ${AUDIO_TOOLBOX}
        ${CORE_AUDIO})
else()
    if(OPENAL_SHARED_LIBS)
        set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal.so)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${OPENAL_LIBRARY}
                ${OPENAL_LIBRARY}.1
                ${OPENAL_LIBRARY}.1.18.2
                DESTINATION lib)
        endif()
    else()
        set(OPENAL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libopenal.a)
    endif()
    set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})
endif()

if(NOT OPENAL_SHARED_LIBS)
    set(OPENAL_DEFINES -DAL_LIBTYPE_STATIC)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OPENAL
    REQUIRED_VARS OPENAL_INCLUDE_DIR OPENAL_LIBRARY)
mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)

if(OPENAL_FOUND AND NOT TARGET OpenAL::OpenAL)
    add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
    set_target_properties(OpenAL::OpenAL PROPERTIES
        IMPORTED_LOCATION "${OPENAL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS "${OPENAL_DEFINES}")
endif()
if(OPENAL_FOUND AND NOT TARGET OpenAL)
    add_library(OpenAL INTERFACE)
    target_link_libraries(OpenAL INTERFACE OpenAL::OpenAL)
endif()



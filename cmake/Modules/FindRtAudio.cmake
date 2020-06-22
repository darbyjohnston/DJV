# Find the RtAudio library.
#
# This module defines the following variables:
#
# * RtAudio_FOUND
# * RtAudio_INCLUDE_DIRS
# * RtAudio_LIBRARIES
#
# This module defines the following imported targets:
#
# * RtAudio::RtAudio
#
# This module defines the following interfaces:
#
# * RtAudio

find_path(RtAudio_INCLUDE_DIR NAMES RtAudio.h PATH_SUFFIXES rtaudio)
set(RtAudio_INCLUDE_DIRS ${RtAudio_INCLUDE_DIR})

if(WIN32)
    set(RtAudio_LINK_LIBRARIES)
elseif(APPLE)
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(CORE_AUDIO CoreAudio)
    set(RtAudio_LINK_LIBRARIES ${AUDIO_TOOLBOX} ${CORE_AUDIO})
    set(RtAudio_LIBRARIES ${RtAudio_LIBRARY} ${RtAudio_LINK_LIBRARIES})
else()
    find_package(ALSA REQUIRED)
    set(RtAudio_INCLUDE_DIRS ${ALSA_INCLUDE_DIR})
    set(RtAudio_LINK_LIBRARIES ${ALSA_LIBRARY})
    set(RtAudio_RPM_PACKAGE_REQUIRES "alsa-lib")
    set(RtAudio_DEBIAN_PACKAGE_DEPENDS "libasound2")
    find_library(PULSE_LIB pulse)
    find_library(PULSESIMPLE_LIB pulse-simple)
    if(PULSE_LIB AND PULSESIMPLE_LIB)
        set(RtAudio_INCLUDE_DIRS ${PULSE_INCLUDE_DIR} ${PULSESIMPLE_INCLUDE_DIR} ${RtAudio_INCLUDE_DIRS})
        set(RtAudio_LINK_LIBRARIES ${PULSE_LIB} ${PULSESIMPLE_LIB} ${RtAudio_LINK_LIBRARIES})
        string(APPEND RtAudio_RPM_PACKAGE_REQUIRES ", pulseaudio-libs")
        string(APPEND RtAudio_DEBIAN_PACKAGE_DEPENDS ", libpulse0")
    endif()
    find_library(JACK_LIB jack)
    if(JACK_LIB)
        set(RtAudio_INCLUDE_DIRS ${JACK_INCLUDE_DIR} ${RtAudio_INCLUDE_DIRS})
        set(RtAudio_LINK_LIBRARIES ${JACK_LIB} ${RtAudio_LINK_LIBRARIES})
    endif()
    set(RtAudio_INCLUDE_DIRS ${RtAudio_INCLUDE_DIR} ${RtAudio_INCLUDE_DIRS})
endif()

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(RtAudio_LIBRARY NAMES rtaudio)
else()
    find_library(RtAudio_LIBRARY NAMES rtaudio)
endif()
set(RtAudio_LIBRARIES ${RtAudio_LIBRARY} ${RtAudio_LINK_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    RtAudio
    REQUIRED_VARS RtAudio_INCLUDE_DIR RtAudio_LIBRARY)
mark_as_advanced(RtAudio_INCLUDE_DIR RtAudio_LIBRARY)

if(RtAudio_FOUND AND NOT TARGET RtAudio::RtAudio)
    add_library(RtAudio::RtAudio UNKNOWN IMPORTED)
    set_target_properties(RtAudio::RtAudio PROPERTIES
        IMPORTED_LOCATION "${RtAudio_LIBRARY}"
        INTERFACE_LINK_LIBRARIES "${RtAudio_LINK_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${RtAudio_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS RtAudio_FOUND)
endif()
if(RtAudio_FOUND AND NOT TARGET RtAudio)
    add_library(RtAudio INTERFACE)
    target_link_libraries(RtAudio INTERFACE RtAudio::RtAudio)
endif()


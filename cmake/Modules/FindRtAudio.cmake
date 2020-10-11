# Find the RtAudio library.
#
# This module defines the following variables:
#
# * RtAudio_FOUND
#
# This module defines the following imported targets:
#
# * RtAudio::RtAudio
#
# This module defines the following interfaces:
#
# * RtAudio

find_path(RtAudio_INCLUDE_DIR NAMES rtaudio/RtAudio.h)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(RtAudio_LIBRARY NAMES rtaudio)
else()
    find_library(RtAudio_LIBRARY NAMES rtaudio)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    RtAudio
    REQUIRED_VARS RtAudio_INCLUDE_DIR RtAudio_LIBRARY)
mark_as_advanced(RtAudio_INCLUDE_DIR RtAudio_LIBRARY)

if(RtAudio_FOUND AND NOT TARGET RtAudio::RtAudio)
    add_library(RtAudio::RtAudio UNKNOWN IMPORTED)
    set_target_properties(RtAudio::RtAudio PROPERTIES
        IMPORTED_LOCATION "${RtAudio_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS RtAudio_FOUND)
    if(WIN32)
    elseif(APPLE)
        find_library(AUDIO_TOOLBOX AudioToolbox)
        find_library(CORE_AUDIO CoreAudio)
		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${AUDIO_TOOLBOX};${CORE_AUDIO}")
    else()
        find_package(ALSA REQUIRED)
		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ALSA_INCLUDE_DIR}")
		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${ALSA_LIBRARY}")
        set(RtAudio_RPM_PACKAGE_REQUIRES "alsa-lib")
        set(RtAudio_DEBIAN_PACKAGE_DEPENDS "libasound2")
        find_library(PULSE_LIB pulse)
        find_library(PULSESIMPLE_LIB pulse-simple)
        if(PULSE_LIB AND PULSESIMPLE_LIB)
    		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${PULSE_INCLUDE_DIR};${PULSESIMPLE_INCLUDE_DIR}")
    		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${PULSE_LIB};${PULSESIMPLE_LIB}")
            string(APPEND RtAudio_RPM_PACKAGE_REQUIRES ", pulseaudio-libs")
            string(APPEND RtAudio_DEBIAN_PACKAGE_DEPENDS ", libpulse0")
        endif()
        find_library(JACK_LIB jack)
        if(JACK_LIB)
    		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${JACK_INCLUDE_DIR}")
    		set_property(TARGET RtAudio::RtAudio APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${JACK_LIB}")
        endif()
    endif()
endif()
if(RtAudio_FOUND AND NOT TARGET RtAudio)
    add_library(RtAudio INTERFACE)
    target_link_libraries(RtAudio INTERFACE RtAudio::RtAudio)
endif()


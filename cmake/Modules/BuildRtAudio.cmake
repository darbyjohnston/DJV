include(ExternalProject)

if(NOT RtAudioThirdParty)
    set(RtAudio_ARGS
        -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_DEBUG_POSTFIX=
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DBUILD_SHARED_LIBS=${RtAudio_SHARED_LIBS}
        -DRTAUDIO_BUILD_TESTING=FALSE)
    if(DJV_BUILD_PACKAGE)
        set(RtAudio_ARGS ${RtAudio_ARGS} -DRTAUDIO_API_JACK=FALSE)
    endif()
    ExternalProject_Add(
        RtAudioThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RtAudio
        GIT_REPOSITORY https://github.com/thestk/rtaudio.git
        CMAKE_ARGS ${RtAudio_ARGS})
endif()

set(RtAudio_FOUND TRUE)
set(RtAudio_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(WIN32)
	set(RtAudio_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/rtaudio${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(RtAudio_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/librtaudio${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
if(WIN32)
    set(RtAudio_LINK_LIBRARIES)
    set(RtAudio_LIBRARIES ${RtAudio_LIBRARY})
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
    elseif(DJV_BUILD_PACKAGE)
        message(FATAL_ERROR "Cannot find the PulseAudio library")
    endif()
    if(NOT DJV_BUILD_PACKAGE)
        find_library(JACK_LIB jack)
        if(JACK_LIB)
            set(RtAudio_INCLUDE_DIRS ${JACK_INCLUDE_DIR} ${RtAudio_INCLUDE_DIRS})
            set(RtAudio_LINK_LIBRARIES ${JACK_LIB} ${RtAudio_LINK_LIBRARIES})
        endif()
    endif()
    set(RtAudio_INCLUDE_DIRS ${RtAudio_INCLUDE_DIR} ${RtAudio_INCLUDE_DIRS})
    set(RtAudio_LIBRARIES ${RtAudio_LIBRARY} ${RtAudio_LINK_LIBRARIES})
endif()

if(RtAudio_FOUND AND NOT TARGET RtAudio::RtAudio)
    add_library(RtAudio::RtAudio UNKNOWN IMPORTED)
    add_dependencies(RtAudio::RtAudio RtAudioThirdParty)
	set_property(TARGET RtAudio::RtAudio PROPERTY IMPORTED_LOCATION ${RtAudio_LIBRARY})
	set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_LINK_LIBRARIES ${RtAudio_LINK_LIBRARIES})
	set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RtAudio_INCLUDE_DIRS})
	set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_COMPILE_DEFINITIONS RtAudio_FOUND)
endif()
if(RtAudio_FOUND AND NOT TARGET RtAudio)
    add_library(RtAudio INTERFACE)
    target_link_libraries(RtAudio INTERFACE RtAudio::RtAudio)
endif()


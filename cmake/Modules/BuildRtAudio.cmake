include(ExternalProject)

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
ExternalProject_Add(
    RtAudioThirdParty
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/RtAudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    CMAKE_ARGS ${RtAudio_ARGS})
    
set(RtAudio_FOUND TRUE)
set(RtAudio_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
if(WIN32)
	set(RtAudio_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/rtaudio${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(RtAudio_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/librtaudio${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
if(WIN32)
    set(RtAudio_LIBRARIES ${RtAudio_LIBRARY})
elseif(APPLE)
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(CORE_AUDIO CoreAudio)
    set(RtAudio_LIBRARIES ${RtAudio_LIBRARY} ${AUDIO_TOOLBOX} ${CORE_AUDIO})
else()
    find_package(ALSA)
    find_library(PULSE_LIB pulse)
    find_library(PULSESIMPLE_LIB pulse-simple)
    set(RtAudio_INCLUDE_DIRS ${RtAudio_INCLUDE_DIRS} ${ALSA_INCLUDE_DIR})
    set(RtAudio_INCLUDE_DIRS ${RtAudio_INCLUDE_DIRS} ${PULSE_INCLUDE_DIR} ${PULSESIMPLE_INCLUDE_DIR})
    set(RtAudio_LIBRARIES ${RtAudio_LIBRARY} ${PULSE_LIB} ${PULSESIMPLE_LIB} ${ALSA_LIBRARY})
endif()

if(RtAudio_FOUND AND NOT TARGET RtAudio::RtAudio)
    add_library(RtAudio::RtAudio UNKNOWN IMPORTED)
    add_dependencies(RtAudio::RtAudio RtAudioThirdParty)
	if(WIN32)
		set_property(TARGET RtAudio::RtAudio PROPERTY IMPORTED_LOCATION ${RtAudio_LIBRARY})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RtAudio_INCLUDE_DIRS})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_COMPILE_DEFINITIONS RtAudio_FOUND)
	elseif(APPLE)
		set_property(TARGET RtAudio::RtAudio PROPERTY IMPORTED_LOCATION ${RtAudio_LIBRARY})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_LINK_LIBRARIES ${PULSE_LIB} ${AUDIO_TOOLBOX} ${CORE_AUDIO})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RtAudio_INCLUDE_DIRS})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_COMPILE_DEFINITIONS RtAudio_FOUND)
	else()
		set_property(TARGET RtAudio::RtAudio PROPERTY IMPORTED_LOCATION ${RtAudio_LIBRARY})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_LINK_LIBRARIES ${PULSE_LIB} ${PULSESIMPLE_LIB} ${ALSA_LIBRARY})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RtAudio_INCLUDE_DIRS})
		set_property(TARGET RtAudio::RtAudio PROPERTY INTERFACE_COMPILE_DEFINITIONS RtAudio_FOUND)
	endif()
endif()
if(RtAudio_FOUND AND NOT TARGET RtAudio)
    add_library(RtAudio INTERFACE)
    target_link_libraries(RtAudio INTERFACE RtAudio::RtAudio)
endif()


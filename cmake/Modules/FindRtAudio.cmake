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

find_path(RtAudio_INCLUDE_DIR NAMES rtaudio/RtAudio.h)
set(RtAudio_INCLUDE_DIRS ${RtAudio_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(RtAudio_LIBRARY NAMES rtaudio)
else()
    find_library(RtAudio_LIBRARY NAMES rtaudio)
endif()
set(RtAudio_LIBRARIES ${RtAudio_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    RtAudio
    REQUIRED_VARS RtAudio_INCLUDE_DIR RtAudio_LIBRARY)
mark_as_advanced(RtAudio_INCLUDE_DIR RtAudio_LIBRARY)

if(RtAudio_FOUND AND NOT TARGET RtAudio::RtAudio)
    add_library(RtAudio::RtAudio UNKNOWN IMPORTED)
    set_target_properties(RtAudio::RtAudio PROPERTIES
        IMPORTED_LOCATION "${RtAudio_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${RtAudio_INCLUDE_DIR}")
endif()
if(RtAudio_FOUND AND NOT TARGET RtAudio)
    add_library(RtAudio INTERFACE)
    target_link_libraries(RtAudio INTERFACE RtAudio::RtAudio)
endif()

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


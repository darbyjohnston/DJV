# Find the FFmpeg library.
#
# This module defines the following variables:
#
# * FFMPEG_FOUND
# * FFMPEG_INCLUDE_DIRS
# * FFMPEG_LIBRARIES
#
# This module defines the following imported targets:
#
# * FFMPEG::avcodec
# * FFMPEG::avdevice
# * FFMPEG::avfilter
# * FFMPEG::avformat
# * FFMPEG::avutil
# * FFMPEG::swresample
# * FFMPEG::swscale
#
# This module defines the following interfaces:
#
# * FFMPEG

find_package(ZLIB REQUIRED)

find_path(FFMPEG_INCLUDE_DIR
    NAMES avcodec.h
	PATH_SUFFIXES libavcodec)
set(FFMPEG_INCLUDE_DIRS
    ${FFMPEG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

find_library(FFMPEG_LIBAVCODEC NAMES avcodec)
find_library(FFMPEG_LIBAVDEVICE NAMES avdevice)
find_library(FFMPEG_LIBAVFILTER NAMES avfilter)
find_library(FFMPEG_LIBAVFORMAT NAMES avformat)
find_library(FFMPEG_LIBAVUTIL NAMES avutil)
find_library(FFMPEG_LIBSWRESAMPLE NAMES swresample)
find_library(FFMPEG_LIBSWSCALE NAMES swscale)
set(FFMPEG_LIBRARIES
    ${FFMPEG_LIBAVFORMAT}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBSWSCALE}
    ${FFMPEG_LIBAVUTIL}
    ${FFMPEG_LIBAVFILTER}
    ${FFMPEG_LIBSWRESAMPLE}
    ${FFMPEG_LIBAVDEVICE}
    ${ZLIB_LIBRARIES})
if(APPLE)
    set(FFMPEG_LIBRARIES
        ${FFMPEG_LIBRARIES}
        "-framework Security"
        "-framework VideoToolbox"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework CoreFoundation")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FFmpeg
    REQUIRED_VARS
        FFMPEG_INCLUDE_DIR
        FFMPEG_LIBAVCODEC
        FFMPEG_LIBAVDEVICE
        FFMPEG_LIBAVFILTER
        FFMPEG_LIBAVFORMAT
        FFMPEG_LIBAVUTIL
        FFMPEG_LIBSWRESAMPLE
        FFMPEG_LIBSWSCALE)
mark_as_advanced(
    FFMPEG_INCLUDE_DIR
    FFMPEG_LIBAVCODEC
    FFMPEG_LIBAVDEVICE
    FFMPEG_LIBAVFILTER
    FFMPEG_LIBAVFORMAT
    FFMPEG_LIBAVUTIL
    FFMPEG_LIBSWRESAMPLE
    FFMPEG_LIBSWSCALE)

if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avcodec)
    add_library(FFMPEG::avcodec UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::avcodec PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVCODEC}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avdevice)
    add_library(FFMPEG::avdevice UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::avdevice PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVDEVICE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avfilter)
    add_library(FFMPEG::avfilter UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::avfilter PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVFILTER}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avformat)
    add_library(FFMPEG::avformat UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::avformat PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVFORMAT}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avutil)
    add_library(FFMPEG::avutil UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::avutil PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVUTIL}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::swresample)
    add_library(FFMPEG::swresample UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::swresample PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBSWRESAMPLE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::swscale)
    add_library(FFMPEG::swscale UNKNOWN IMPORTED)
    set_target_properties(FFMPEG::swscale PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBSWSCALE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG)
    add_library(FFMPEG INTERFACE)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avcodec)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avdevice)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avfilter)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avformat)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avutil)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::swresample)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::swscale)
    target_link_libraries(FFMPEG INTERFACE ZLIB::ZLIB)
endif()


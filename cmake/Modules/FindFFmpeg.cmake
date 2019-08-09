# Find the FFmpeg library.
#
# This module defines the following variables:
#
# * FFmpeg_FOUND
# * FFmpeg_INCLUDE_DIRS
# * FFmpeg_LIBRARIES
#
# This module defines the following imported targets:
#
# * FFmpeg::avcodec
# * FFmpeg::avdevice
# * FFmpeg::avfilter
# * FFmpeg::avformat
# * FFmpeg::avutil
# * FFmpeg::swresample
# * FFmpeg::swscale
#
# This module defines the following interfaces:
#
# * FFmpeg

find_package(ZLIB REQUIRED)

find_path(FFmpeg_INCLUDE_DIR
    NAMES avcodec.h
	PATH_SUFFIXES libavcodec)
set(FFmpeg_INCLUDE_DIRS
    ${FFmpeg_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

find_library(FFmpeg_LIBAVCODEC NAMES avcodec)
find_library(FFmpeg_LIBAVDEVICE NAMES avdevice)
find_library(FFmpeg_LIBAVFILTER NAMES avfilter)
find_library(FFmpeg_LIBAVFORMAT NAMES avformat)
find_library(FFmpeg_LIBAVUTIL NAMES avutil)
find_library(FFmpeg_LIBSWRESAMPLE NAMES swresample)
find_library(FFmpeg_LIBSWSCALE NAMES swscale)
set(FFmpeg_LIBRARIES
    ${FFmpeg_LIBAVFORMAT}
    ${FFmpeg_LIBAVCODEC}
    ${FFmpeg_LIBSWSCALE}
    ${FFmpeg_LIBAVUTIL}
    ${FFmpeg_LIBAVFILTER}
    ${FFmpeg_LIBSWRESAMPLE}
    ${FFmpeg_LIBAVDEVICE}
    ${ZLIB_LIBRARIES})
if(APPLE)
    set(FFmpeg_LIBRARIES
        ${FFmpeg_LIBRARIES}
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
        FFmpeg_INCLUDE_DIR
        FFmpeg_LIBAVCODEC
        FFmpeg_LIBAVDEVICE
        FFmpeg_LIBAVFILTER
        FFmpeg_LIBAVFORMAT
        FFmpeg_LIBAVUTIL
        FFmpeg_LIBSWRESAMPLE
        FFmpeg_LIBSWSCALE)
mark_as_advanced(
    FFmpeg_INCLUDE_DIR
    FFmpeg_LIBAVCODEC
    FFmpeg_LIBAVDEVICE
    FFmpeg_LIBAVFILTER
    FFmpeg_LIBAVFORMAT
    FFmpeg_LIBAVUTIL
    FFmpeg_LIBSWRESAMPLE
    FFmpeg_LIBSWSCALE)

if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avcodec)
    add_library(FFmpeg::avcodec UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avcodec PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVCODEC}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avdevice)
    add_library(FFmpeg::avdevice UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avdevice PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVDEVICE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avfilter)
    add_library(FFmpeg::avfilter UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avfilter PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVFILTER}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avformat)
    add_library(FFmpeg::avformat UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avformat PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVFORMAT}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avutil)
    add_library(FFmpeg::avutil UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::avutil PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVUTIL}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::swresample)
    add_library(FFmpeg::swresample UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::swresample PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBSWRESAMPLE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::swscale)
    add_library(FFmpeg::swscale UNKNOWN IMPORTED)
    set_target_properties(FFmpeg::swscale PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBSWSCALE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg)
    add_library(FFmpeg INTERFACE)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avcodec)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avdevice)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avfilter)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avformat)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avutil)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::swresample)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::swscale)
    target_link_libraries(FFmpeg INTERFACE ZLIB::ZLIB)
endif()


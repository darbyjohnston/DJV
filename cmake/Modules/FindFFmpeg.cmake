# Find the FFmpeg library.
#
# This module defines the following variables:
#
# * FFMPEG_INCLUDE_DIRS - Include directories for FFmpeg.
# * FFMPEG_LIBRARIES    - Libraries to link against FFmpeg.
# * FFMPEG_FOUND        - True if FFmpeg has been found and can be used.

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_path(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h)
find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)

set(FFMPEG_INCLUDE_DIRS
    ${AVUTIL_INCLUDE_DIR}
    ${AVDEVICE_INCLUDE_DIR}
    ${AVFILTER_INCLUDE_DIR}
    ${AVFORMAT_INCLUDE_DIR}
    ${AVUTIL_INCLUDE_DIR}
    ${SWRESAMPLE_INCLUDE_DIR}
    ${SWSCALE_INCLUDE_DIR})

set(findLibraryOptions)

find_library(
    AVCODEC_LIBRARY
    NAMES avcodec
    PATH_SUFFIXES lib
    ${findLibraryOptions})
find_library(
    AVDEVICE_LIBRARY
    NAMES avdevice
    PATH_SUFFIXES lib
    ${findLibraryOptions})
find_library(
    AVFILTER_LIBRARY
    NAMES avfilter
    PATH_SUFFIXES lib
    ${findLibraryOptions})
find_library(
    AVFORMAT_LIBRARY
    NAMES avformat
    PATH_SUFFIXES lib
    ${findLibraryOptions})
find_library(
    AVUTIL_LIBRARY
    NAMES avutil
    PATH_SUFFIXES lib
    ${findLibraryOptions})
find_library(
    SWRESAMPLE_LIBRARY
    NAMES swresample
    PATH_SUFFIXES lib
    ${findLibraryOptions})
find_library(
    SWSCALE_LIBRARY
    NAMES swscale
    PATH_SUFFIXES lib
    ${findLibraryOptions})

set(FFMPEG_LIBRARIES
    ${AVCODEC_LIBRARY}
    ${AVDEVICE_LIBRARY}
    ${AVFILTER_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWRESAMPLE_LIBRARY}
    ${SWSCALE_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    FFMPEG
    REQUIRED_VARS
        AVCODEC_INCLUDE_DIR
        AVDEVICE_INCLUDE_DIR
        AVFILTER_INCLUDE_DIR
        AVFORMAT_INCLUDE_DIR
        AVUTIL_INCLUDE_DIR
        SWRESAMPLE_INCLUDE_DIR
        SWSCALE_INCLUDE_DIR
        AVCODEC_LIBRARY
        AVDEVICE_LIBRARY
        AVFILTER_LIBRARY
        AVFORMAT_LIBRARY
        AVUTIL_LIBRARY
        SWRESAMPLE_LIBRARY
        SWSCALE_LIBRARY)

mark_as_advanced(
    AVCODEC_INCLUDE_DIR
    AVDEVICE_INCLUDE_DIR
    AVFILTER_INCLUDE_DIR
    AVFORMAT_INCLUDE_DIR
    AVUTIL_INCLUDE_DIR
    SWRESAMPLE_INCLUDE_DIR
    SWSCALE_INCLUDE_DIR
    AVCODEC_LIBRARY
    AVDEVICE_LIBRARY
    AVFILTER_LIBRARY
    AVFORMAT_LIBRARY
    AVUTIL_LIBRARY
    SWRESAMPLE_LIBRARY
    SWSCALE_LIBRARY)


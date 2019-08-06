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

if(DJV_THIRD_PARTY)
    if(WIN32)
        install(
            FILES
            ${DJV_THIRD_PARTY}/bin/avcodec-58.dll
            ${DJV_THIRD_PARTY}/bin/avdevice-58.dll
            ${DJV_THIRD_PARTY}/bin/avfilter-7.dll
            ${DJV_THIRD_PARTY}/bin/avformat-58.dll
            ${DJV_THIRD_PARTY}/bin/avutil-56.dll
            ${DJV_THIRD_PARTY}/bin/swresample-3.dll
            ${DJV_THIRD_PARTY}/bin/swscale-5.dll
            DESTINATION ${DJV_INSTALL_BIN})
    elseif(APPLE)
        install(
            FILES
            ${FFMPEG_LIBAVCODEC}
            ${DJV_THIRD_PARTY}/lib/libavcodec.58.dylib
            ${DJV_THIRD_PARTY}/lib/libavcodec.58.54.100.dylib
            ${FFMPEG_LIBAVDEVICE}
            ${DJV_THIRD_PARTY}/lib/libavdevice.58.dylib
            ${DJV_THIRD_PARTY}/lib/libavdevice.58.8.100.dylib
            ${FFMPEG_LIBAVFILTER}
            ${DJV_THIRD_PARTY}/lib/libavfilter.7.dylib
            ${DJV_THIRD_PARTY}/lib/libavfilter.7.57.100.dylib
            ${FFMPEG_LIBAVFORMAT}
            ${DJV_THIRD_PARTY}/lib/libavformat.58.dylib
            ${DJV_THIRD_PARTY}/lib/libavformat.58.29.100.dylib
            ${FFMPEG_LIBAVUTIL}
            ${DJV_THIRD_PARTY}/lib/libavutil.56.dylib
            ${DJV_THIRD_PARTY}/lib/libavutil.56.31.100.dylib
            ${FFMPEG_LIBSWRESAMPLE}
            ${DJV_THIRD_PARTY}/lib/libswresample.3.dylib
            ${DJV_THIRD_PARTY}/lib/libswresample.3.5.100.dylib
            ${FFMPEG_LIBSWSCALE}
            ${DJV_THIRD_PARTY}/lib/libswscale.5.dylib
            ${DJV_THIRD_PARTY}/lib/libswscale.5.5.100.dylib
            DESTINATION ${DJV_INSTALL_LIB})
    else()
        install(
            FILES
            ${FFMPEG_LIBAVCODEC}
            ${FFMPEG_LIBAVCODEC}.58
            ${FFMPEG_LIBAVCODEC}.58.54.100
            ${FFMPEG_LIBAVDEVICE}
            ${FFMPEG_LIBAVDEVICE}.58
            ${FFMPEG_LIBAVDEVICE}.58.8.100
            ${FFMPEG_LIBAVFILTER}
            ${FFMPEG_LIBAVFILTER}.7
            ${FFMPEG_LIBAVFILTER}.7.57.100
            ${FFMPEG_LIBAVFORMAT}
            ${FFMPEG_LIBAVFORMAT}.58
            ${FFMPEG_LIBAVFORMAT}.58.29.100
            ${FFMPEG_LIBAVUTIL}
            ${FFMPEG_LIBAVUTIL}.56
            ${FFMPEG_LIBAVUTIL}.56.31.100
            ${FFMPEG_LIBSWRESAMPLE}
            ${FFMPEG_LIBSWRESAMPLE}.3
            ${FFMPEG_LIBSWRESAMPLE}.3.5.100
            ${FFMPEG_LIBSWSCALE}
            ${FFMPEG_LIBSWSCALE}.5
            ${FFMPEG_LIBSWSCALE}.5.5.100
            DESTINATION ${DJV_INSTALL_LIB})
    endif()
endif()


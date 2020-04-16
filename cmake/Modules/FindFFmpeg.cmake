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

if(FFmpeg_FOUND)
	if(WIN32)
		install(
			FILES
			${CMAKE_PREFIX_PATH}/bin/avcodec-58.dll
			${CMAKE_PREFIX_PATH}/bin/avdevice-58.dll
			${CMAKE_PREFIX_PATH}/bin/avfilter-7.dll
			${CMAKE_PREFIX_PATH}/bin/avformat-58.dll
			${CMAKE_PREFIX_PATH}/bin/avutil-56.dll
			${CMAKE_PREFIX_PATH}/bin/swresample-3.dll
			${CMAKE_PREFIX_PATH}/bin/swscale-5.dll
			DESTINATION bin)
	elseif(APPLE)
		install(
			FILES
			${FFmpeg_LIBAVCODEC}
			${CMAKE_PREFIX_PATH}/lib/libavcodec.58.dylib
			${CMAKE_PREFIX_PATH}/lib/libavcodec.58.54.100.dylib
			${FFmpeg_LIBAVDEVICE}
			${CMAKE_PREFIX_PATH}/lib/libavdevice.58.dylib
			${CMAKE_PREFIX_PATH}/lib/libavdevice.58.8.100.dylib
			${FFmpeg_LIBAVFILTER}
			${CMAKE_PREFIX_PATH}/lib/libavfilter.7.dylib
			${CMAKE_PREFIX_PATH}/lib/libavfilter.7.57.100.dylib
			${FFmpeg_LIBAVFORMAT}
			${CMAKE_PREFIX_PATH}/lib/libavformat.58.dylib
			${CMAKE_PREFIX_PATH}/lib/libavformat.58.29.100.dylib
			${FFmpeg_LIBAVUTIL}
			${CMAKE_PREFIX_PATH}/lib/libavutil.56.dylib
			${CMAKE_PREFIX_PATH}/lib/libavutil.56.31.100.dylib
			${FFmpeg_LIBSWRESAMPLE}
			${CMAKE_PREFIX_PATH}/lib/libswresample.3.dylib
			${CMAKE_PREFIX_PATH}/lib/libswresample.3.5.100.dylib
			${FFmpeg_LIBSWSCALE}
			${CMAKE_PREFIX_PATH}/lib/libswscale.5.dylib
			${CMAKE_PREFIX_PATH}/lib/libswscale.5.5.100.dylib
			DESTINATION lib)
	else()
		install(
			FILES
			${FFmpeg_LIBAVCODEC}
			${FFmpeg_LIBAVCODEC}.58
			${FFmpeg_LIBAVCODEC}.58.54.100
			${FFmpeg_LIBAVDEVICE}
			${FFmpeg_LIBAVDEVICE}.58
			${FFmpeg_LIBAVDEVICE}.58.8.100
			${FFmpeg_LIBAVFILTER}
			${FFmpeg_LIBAVFILTER}.7
			${FFmpeg_LIBAVFILTER}.7.57.100
			${FFmpeg_LIBAVFORMAT}
			${FFmpeg_LIBAVFORMAT}.58
			${FFmpeg_LIBAVFORMAT}.58.29.100
			${FFmpeg_LIBAVUTIL}
			${FFmpeg_LIBAVUTIL}.56
			${FFmpeg_LIBAVUTIL}.56.31.100
			${FFmpeg_LIBSWRESAMPLE}
			${FFmpeg_LIBSWRESAMPLE}.3
			${FFmpeg_LIBSWRESAMPLE}.3.5.100
			${FFmpeg_LIBSWSCALE}
			${FFmpeg_LIBSWSCALE}.5
			${FFmpeg_LIBSWSCALE}.5.5.100
			DESTINATION lib)
	endif()
endif()

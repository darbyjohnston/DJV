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

set(FFMPEG_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(FFMPEG_INCLUDE_DIRS
    ${FFMPEG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS})

if(WIN32)
    if(FFMPEG_SHARED_LIBS)
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/bin/avcodec.lib)
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/bin/avdevice.lib)
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/bin/avfilter.lib)
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/bin/avformat.lib)
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/bin/avutil.lib)
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/bin/swresample.lib)
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/bin/swscale.lib)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${CMAKE_INSTALL_PREFIX}/bin/avcodec-58.dll
                ${CMAKE_INSTALL_PREFIX}/bin/avdevice-58.dll
                ${CMAKE_INSTALL_PREFIX}/bin/avfilter-7.dll
                ${CMAKE_INSTALL_PREFIX}/bin/avformat-58.dll
                ${CMAKE_INSTALL_PREFIX}/bin/avutil-56.dll
                ${CMAKE_INSTALL_PREFIX}/bin/swresample-3.dll
                ${CMAKE_INSTALL_PREFIX}/bin/swscale-5.dll
                DESTINATION bin)
        endif()
    else()
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.a)
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.a)
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.a)
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat.a)
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil.a)
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample.a)
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale.a)
    endif()
elseif(APPLE)
    if(FFMPEG_SHARED_LIBS)
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.dylib)
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.dylib)
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.dylib)
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat.dylib)
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil.dylib)
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample.dylib)
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale.dylib)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${FFMPEG_LIBAVCODEC}
                ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.58.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.58.18.100.dylib
                ${FFMPEG_LIBAVDEVICE}
                ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.58.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.58.3.100.dylib
                ${FFMPEG_LIBAVFILTER}
                ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.7.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.7.16.100.dylib
                ${FFMPEG_LIBAVFORMAT}
                ${CMAKE_INSTALL_PREFIX}/lib/libavformat.58.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libavformat.58.12.100.dylib
                ${FFMPEG_LIBAVUTIL}
                ${CMAKE_INSTALL_PREFIX}/lib/libavutil.56.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libavutil.56.14.100.dylib
                ${FFMPEG_LIBSWRESAMPLE}
                ${CMAKE_INSTALL_PREFIX}/lib/libswresample.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libswresample.3.1.100.dylib
                ${FFMPEG_LIBSWSCALE}
                ${CMAKE_INSTALL_PREFIX}/lib/libswscale.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libswscale.5.1.100.dylib
                DESTINATION lib)
        endif()
    else()
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.a)
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.a)
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.a)
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat.a)
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil.a)
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample.a)
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale.a)
    endif()
    set(FFMPEG_LIBRARIES
        ${FFMPEG_LIBAVFORMAT}
        ${FFMPEG_LIBAVCODEC}
        ${FFMPEG_LIBSWSCALE}
        ${FFMPEG_LIBAVUTIL}
        ${FFMPEG_LIBAVFILTER}
        ${FFMPEG_LIBSWRESAMPLE}
        ${FFMPEG_LIBAVDEVICE}
        ${ZLIB_LIBRARIES}
        "-framework Security"
        "-framework VideoToolbox"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework CoreFoundation")
else()
    if(FFMPEG_SHARED_LIBS)
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so)
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so)
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.so)
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so)
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so)
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so)
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${FFMPEG_LIBAVCODEC}
                ${FFMPEG_LIBAVCODEC}.58
                ${FFMPEG_LIBAVCODEC}.58.18.100
                ${FFMPEG_LIBAVDEVICE}
                ${FFMPEG_LIBAVDEVICE}.58
                ${FFMPEG_LIBAVDEVICE}.58.3.100
                ${FFMPEG_LIBAVFILTER}
                ${FFMPEG_LIBAVFILTER}.7
                ${FFMPEG_LIBAVFILTER}.7.16.100
                ${FFMPEG_LIBAVFORMAT}
                ${FFMPEG_LIBAVFORMAT}.58
                ${FFMPEG_LIBAVFORMAT}.58.12.100
                ${FFMPEG_LIBAVUTIL}
                ${FFMPEG_LIBAVUTIL}.56
                ${FFMPEG_LIBAVUTIL}.56.14.100
                ${FFMPEG_LIBSWRESAMPLE}
                ${FFMPEG_LIBSWRESAMPLE}.3
                ${FFMPEG_LIBSWRESAMPLE}.3.1.100
                ${FFMPEG_LIBSWSCALE}
                ${FFMPEG_LIBSWSCALE}.5
                ${FFMPEG_LIBSWSCALE}.5.1.100
                DESTINATION lib)
        endif()
    else()
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.a)
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.a)
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter.a)
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat.a)
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil.a)
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample.a)
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale.a)
    endif()
    set(FFMPEG_LIBRARIES
        ${FFMPEG_LIBAVFORMAT}
        ${FFMPEG_LIBAVCODEC}
        ${FFMPEG_LIBSWSCALE}
        ${FFMPEG_LIBAVUTIL}
        ${FFMPEG_LIBAVFILTER}
        ${FFMPEG_LIBSWRESAMPLE}
        ${FFMPEG_LIBAVDEVICE}
        ${ZLIB_LIBRARIES})
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


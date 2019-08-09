include(ExternalProject)

set(FFmpeg_EXTRA_CFLAGS)
set(FFmpeg_EXTRA_CXXFLAGS)
set(FFmpeg_EXTRA_LDFLAGS)
set(FFmpeg_CONFIGURE_ARGS
    --prefix=${CMAKE_INSTALL_PREFIX}
    --disable-programs
    --disable-bzlib
    --disable-iconv
    --disable-lzma
    --disable-appkit
    --disable-avfoundation
    --disable-coreimage
    --disable-audiotoolbox
    --enable-pic
    --extra-cflags=${FFmpeg_EXTRA_CFLAGS}
    --extra-cxxflags=${FFmpeg_EXTRA_CXXFLAGS}
    --extra-ldflags=${FFmpeg_EXTRA_LDFLAGS})
if(FFmpeg_SHARED_LIBS)
    set(FFmpeg_CONFIGURE_ARGS ${FFmpeg_CONFIGURE_ARGS} --disable-static --enable-shared)
endif()
ExternalProject_Add(
    FFmpegThirdParty
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/FFmpeg
    DEPENDS ZLIBThirdParty
    URL http://ffmpeg.org/releases/ffmpeg-4.2.tar.bz2
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure ${FFmpeg_CONFIGURE_ARGS}
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install)

set(FFmpeg_FOUND TRUE)
set(FFmpeg_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    if(WIN32)
    else()
        set(FFmpeg_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
else()
    if(WIN32)
    else()
        set(FFmpeg_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFmpeg_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
endif()
set(FFmpeg_LIBRARIES
    ${FFmpeg_LIBAVFORMAT}
    ${FFmpeg_LIBAVCODEC}
    ${FFmpeg_LIBAVDEVICE}
    ${FFmpeg_LIBAVFILTER}
    ${FFmpeg_LIBAVUTIL}
    ${FFmpeg_LIBSWSCALE}
    ${FFmpeg_LIBSWRESAMPLE}
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

if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avcodec)
    add_library(FFmpeg::avcodec UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::avcodec FFmpegThirdParty)
    set_target_properties(FFmpeg::avcodec PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVCODEC}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avdevice)
    add_library(FFmpeg::avdevice UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::avdevice FFmpegThirdParty)
    set_target_properties(FFmpeg::avdevice PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVDEVICE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avfilter)
    add_library(FFmpeg::avfilter UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::avfilter FFmpegThirdParty)
    set_target_properties(FFmpeg::avfilter PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVFILTER}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avformat)
    add_library(FFmpeg::avformat UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::avformat FFmpegThirdParty)
    set_target_properties(FFmpeg::avformat PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVFORMAT}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::avutil)
    add_library(FFmpeg::avutil UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::avutil FFmpegThirdParty)
    set_target_properties(FFmpeg::avutil PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBAVUTIL}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::swscale)
    add_library(FFmpeg::swscale UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::swscale FFmpegThirdParty)
    set_target_properties(FFmpeg::swscale PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBSWSCALE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg::swresample)
    add_library(FFmpeg::swresample UNKNOWN IMPORTED)
    add_dependencies(FFmpeg::swresample FFmpegThirdParty)
    set_target_properties(FFmpeg::swresample PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_LIBSWRESAMPLE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFmpeg_FOUND)
endif()
if(FFmpeg_FOUND AND NOT TARGET FFmpeg)
    add_library(FFmpeg INTERFACE)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avformat)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avcodec)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avdevice)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avfilter)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::avutil)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::swscale)
    target_link_libraries(FFmpeg INTERFACE FFmpeg::swresample)
    target_link_libraries(FFmpeg INTERFACE ZLIB::ZLIB)
endif()

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
        ${FFmpeg_LIBAVCODEC}
        ${DJV_THIRD_PARTY}/lib/libavcodec.58.dylib
        ${DJV_THIRD_PARTY}/lib/libavcodec.58.54.100.dylib
        ${FFmpeg_LIBAVDEVICE}
        ${DJV_THIRD_PARTY}/lib/libavdevice.58.dylib
        ${DJV_THIRD_PARTY}/lib/libavdevice.58.8.100.dylib
        ${FFmpeg_LIBAVFILTER}
        ${DJV_THIRD_PARTY}/lib/libavfilter.7.dylib
        ${DJV_THIRD_PARTY}/lib/libavfilter.7.57.100.dylib
        ${FFmpeg_LIBAVFORMAT}
        ${DJV_THIRD_PARTY}/lib/libavformat.58.dylib
        ${DJV_THIRD_PARTY}/lib/libavformat.58.29.100.dylib
        ${FFmpeg_LIBAVUTIL}
        ${DJV_THIRD_PARTY}/lib/libavutil.56.dylib
        ${DJV_THIRD_PARTY}/lib/libavutil.56.31.100.dylib
        ${FFmpeg_LIBSWRESAMPLE}
        ${DJV_THIRD_PARTY}/lib/libswresample.3.dylib
        ${DJV_THIRD_PARTY}/lib/libswresample.3.5.100.dylib
        ${FFmpeg_LIBSWSCALE}
        ${DJV_THIRD_PARTY}/lib/libswscale.5.dylib
        ${DJV_THIRD_PARTY}/lib/libswscale.5.5.100.dylib
        DESTINATION ${DJV_INSTALL_LIB})
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
        DESTINATION ${DJV_INSTALL_LIB})
endif()


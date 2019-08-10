include(ExternalProject)

set(FFMPEG_EXTRA_CFLAGS)
set(FFMPEG_EXTRA_CXXFLAGS)
set(FFMPEG_EXTRA_LDFLAGS)
set(FFMPEG_CONFIGURE_ARGS
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
    --extra-cflags=${FFMPEG_EXTRA_CFLAGS}
    --extra-cxxflags=${FFMPEG_EXTRA_CXXFLAGS}
    --extra-ldflags=${FFMPEG_EXTRA_LDFLAGS})
if(FFMPEG_SHARED_LIBS)
    set(FFMPEG_CONFIGURE_ARGS ${FFMPEG_CONFIGURE_ARGS} --disable-static --enable-shared)
endif()
ExternalProject_Add(
    FFMPEG_EXTERNAL
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/FFMPEG
    DEPENDS ZLIB_EXTERNAL
    URL http://ffmpeg.org/releases/ffmpeg-4.2.tar.bz2
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure ${FFMPEG_CONFIGURE_ARGS}
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install)

set(FFMPEG_FOUND TRUE)
set(FFMPEG_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    if(WIN32)
    else()
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
else()
    if(WIN32)
    else()
        set(FFMPEG_LIBAVFORMAT ${CMAKE_INSTALL_PREFIX}/lib/libavformat${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVCODEC ${CMAKE_INSTALL_PREFIX}/lib/libavcodec${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVDEVICE ${CMAKE_INSTALL_PREFIX}/lib/libavdevice${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVFILTER ${CMAKE_INSTALL_PREFIX}/lib/libavfilter${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBAVUTIL ${CMAKE_INSTALL_PREFIX}/lib/libavutil${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBSWSCALE ${CMAKE_INSTALL_PREFIX}/lib/libswscale${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(FFMPEG_LIBSWRESAMPLE ${CMAKE_INSTALL_PREFIX}/lib/libswresample${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
endif()
set(FFMPEG_LIBRARIES
    ${FFMPEG_LIBAVFORMAT}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBAVDEVICE}
    ${FFMPEG_LIBAVFILTER}
    ${FFMPEG_LIBAVUTIL}
    ${FFMPEG_LIBSWSCALE}
    ${FFMPEG_LIBSWRESAMPLE}
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

if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avcodec)
    add_library(FFMPEG::avcodec UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::avcodec FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::avcodec PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVCODEC}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avdevice)
    add_library(FFMPEG::avdevice UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::avdevice FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::avdevice PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVDEVICE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avfilter)
    add_library(FFMPEG::avfilter UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::avfilter FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::avfilter PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVFILTER}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avformat)
    add_library(FFMPEG::avformat UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::avformat FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::avformat PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVFORMAT}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::avutil)
    add_library(FFMPEG::avutil UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::avutil FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::avutil PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBAVUTIL}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::swscale)
    add_library(FFMPEG::swscale UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::swscale FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::swscale PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBSWSCALE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG::swresample)
    add_library(FFMPEG::swresample UNKNOWN IMPORTED)
    add_dependencies(FFMPEG::swresample FFMPEG_EXTERNAL)
    set_target_properties(FFMPEG::swresample PROPERTIES
        IMPORTED_LOCATION "${FFMPEG_LIBSWRESAMPLE}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS FFMPEG_FOUND)
endif()
if(FFMPEG_FOUND AND NOT TARGET FFMPEG)
    add_library(FFMPEG INTERFACE)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avformat)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avcodec)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avdevice)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avfilter)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::avutil)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::swscale)
    target_link_libraries(FFMPEG INTERFACE FFMPEG::swresample)
    target_link_libraries(FFMPEG INTERFACE ZLIB::ZLIB)
endif()


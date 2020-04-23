include(ExternalProject)

set(FFmpeg_DEPS ZLIB)
if(WIN32)
else()
    set(FFmpeg_DEPS ${FFmpeg_DEPS} NASM)
endif()

if(WIN32)
    # See the directions for building FFmpeg on Windows in "docs/build_windows.html".
else()
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
        FFmpeg
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/FFmpeg
        DEPENDS ${FFmpeg_DEPS}
        URL http://ffmpeg.org/releases/ffmpeg-4.2.2.tar.bz2
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure ${FFmpeg_CONFIGURE_ARGS})
endif()


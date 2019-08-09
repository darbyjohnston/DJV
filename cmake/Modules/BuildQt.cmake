include(ExternalProject)

set(QT_CONFIGURE_ARGS
    -prefix ${CMAKE_INSTALL_PREFIX}
    -opensource
    -confirm-license
    -opengl desktop
    -qt-zlib
    -qt-libpng
    -qt-libjpeg
    -qt-freetype
    -qt-harfbuzz
    -qt-pcre
    -no-cups
    -no-gif
    -no-icu
    -nomake examples)
if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    set(QT_CONFIGURE_ARGS
        ${QT_CONFIGURE_ARGS}
        -debug)
else()
    set(QT_CONFIGURE_ARGS
        ${QT_CONFIGURE_ARGS}
        -release)
endif()
if(APPLE)
    set(QT_CONFIGURE_ARGS
        ${QT_CONFIGURE_ARGS}
        -no-glib
        -no-eglfs
        -no-linuxfb
        -no-xcb
        -no-xcb-xlib
        -no-framework)
elseif(UNIX)
    set(QT_CONFIGURE_ARGS
        ${QT_CONFIGURE_ARGS}
        -no-glib
        -no-eglfs
        -no-linuxfb        
        -qt-xcb)
endif()

if(WIN32)
    ExternalProject_Add(
        Qt
        DEPENDS ZLIB JPEG PNG FreeType
		URL http://download.qt.io/archive/qt/5.12/5.12.3/submodules/qtbase-everywhere-src-5.12.3.tar.xz
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND call configure.bat ${QT_CONFIGURE_ARGS}
        BUILD_COMMAND nmake
        INSTALL_COMMAND nmake install)
else()
    ExternalProject_Add(
        Qt
        DEPENDS ZLIB JPEG PNG FreeType
		URL http://download.qt.io/archive/qt/5.12/5.12.3/submodules/qtbase-everywhere-src-5.12.3.tar.xz
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure ${QT_CONFIGURE_ARGS}
        BUILD_COMMAND $(MAKE)
        INSTALL_COMMAND $(MAKE) install)
endif()

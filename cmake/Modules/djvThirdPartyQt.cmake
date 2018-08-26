find_package(Qt5Core)
if (Qt5Core_DIR)
    find_package(Qt5LinguistTools REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
    find_package(Qt5Svg REQUIRED)
    find_package(Qt5Widgets REQUIRED)
else()
    find_package(Qt4 REQUIRED)
endif()

if (djvPackageThirdParty)

    if (WIN32)

	    install(
		    FILES
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5CLucene.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Concurrent.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Core.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5EglDeviceIntegration.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Gui.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Network.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5OpenGLExtensions.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5OpenGL.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5PlatformSupport.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5PrintSupport.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Sql.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Svg.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Test.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll
            $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Xml.dll
		    DESTINATION bin)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/qgenericbearer.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/qnativewifibearer.dll
            DESTINATION bin/bearer)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/qico.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/qgif.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/qjpeg.dll
            DESTINATION bin/imageformats)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/qminimal.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/qoffscreen.dll
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll
            DESTINATION bin/platforms)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/printsupport/windowsprintersupport.dll
            DESTINATION bin/printsupport)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/qsqlite.dll
            DESTINATION bin/sqldrivers)

    elseif (APPLE)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.6.3.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.6.dylib
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.6.3.dylib
            DESTINATION lib)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqcorewlanbearer.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqgenericbearer.dylib
            DESTINATION bin/bearer)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqico.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqgif.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqjpeg.dylib
            DESTINATION bin/imageformats)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqcocoa.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqminimal.dylib
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqoffscreen.dylib
            DESTINATION bin/platforms)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/printsupport/libcocoaprintersupport.dylib
            DESTINATION bin/printsupport)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlite.dylib
            DESTINATION bin/sqldrivers)
        
    elseif (UNIX)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5CLucene.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5EglDeviceIntegration.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5EglDeviceIntegration.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5EglDeviceIntegration.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5EglDeviceIntegration.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGLExtensions.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PlatformSupport.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Svg.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5WaylandClient.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5WaylandClient.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5WaylandClient.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5WaylandClient.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5XcbQpa.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5XcbQpa.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5XcbQpa.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5XcbQpa.so.5.6.3
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5.6
            $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5.6.3
            DESTINATION lib)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqconnmanbearer.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqnmbearer.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqgenericbearer.so
            DESTINATION bin/bearer)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/egldeviceintegrations/libqeglfs-kms-egldevice-integration.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/egldeviceintegrations/libqeglfs-x11-integration.so
            DESTINATION bin/egldeviceintegrations)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevkeyboardplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevmouseplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevtabletplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevtouchplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqtuiotouchplugin.so.so
            DESTINATION bin/generic)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/iconengines/libqsvgicon.so.so
            DESTINATION bin/iconengines)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqicns.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqico.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqjpeg.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqmng.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqsvg.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqtga.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqtiff.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqwbmp.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqwebp.so
            DESTINATION bin/imageformats)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
            DESTINATION bin/platforminputcontexts)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqeglfs.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqlinuxfb.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqminimalegl.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqminimal.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqoffscreen.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqwayland-egl.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqwayland-generic.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqxcb.so
            DESTINATION bin/platforms)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platformthemes/libqgtk2.so
            DESTINATION bin/platformthemes)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlite.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlmysql.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlodbc.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlpsql.so
            DESTINATION bin/sqldrivers)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
            DESTINATION bin/platforminputcontexts)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/wayland-decoration-client/libbradient.so
            DESTINATION bin/wayland-decoration-client)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/wayland-graphics-integration-client/libdrm-egl-server.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/wayland-graphics-integration-client/libwayland-egl.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/wayland-graphics-integration-client/libxcomposite-egl.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/wayland-graphics-integration-client/libxcomposite-glx.so
            DESTINATION bin/wayland-graphics-integration-client)

        install(
            FILES
            $ENV{CMAKE_PREFIX_PATH}/plugins/xcbglintegrations/libdrm-egl-server.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/xcbglintegrations/libwayland-egl.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/xcbglintegrations/libxcomposite-egl.so
            $ENV{CMAKE_PREFIX_PATH}/plugins/xcbglintegrations/libxcomposite-glx.so
            DESTINATION bin/platforms)
        
    endif (WIN32)

endif (djvPackageThirdParty)


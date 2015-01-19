find_package(Qt5Core)
if (Qt5Core_DIR)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5OpenGL REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
else()
    find_package(Qt4 REQUIRED)
endif()

if (djvPackageThirdParty)

    if (WIN32)

        if (Qt5Core_DIR)
        
		    install(
			    FILES
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Concurrent.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Core.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Gui.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Network.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5OpenGL.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5PrintSupport.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Sql.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Test.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll
                $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Xml.dll
			    DESTINATION bin)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/qtaccessiblewidgets.dll
                DESTINATION bin/accessible)

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

        else()

            # \todo
        
        endif()

    elseif (APPLE)

        if (Qt5Core_DIR)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.5.3.2.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.3.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.5.3.2.dylib
                DESTINATION lib)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/libqtaccessiblewidgets.dylib
                DESTINATION bin/accessible)

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

        else()
        
            # \todo
        
        endif()
        
    elseif (UNIX)

        if (Qt5Core_DIR)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Concurrent.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Core.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5DBus.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Gui.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Network.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5OpenGL.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5PrintSupport.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Sql.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Test.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Widgets.so.5.3.2
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5.3
                $ENV{CMAKE_PREFIX_PATH}/lib/libQt5Xml.so.5.3.2
                DESTINATION lib)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqconnmanbearer.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqnmbearer.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqgenericbearer.so
                DESTINATION bin/bearer)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqico.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqgif.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqjpeg.so
                DESTINATION bin/imageformats)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/libqtaccessiblewidgets.so
                DESTINATION bin/accessible)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/platformthemes/libqgtk2.so
                DESTINATION bin/platformthemes)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlpsql.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlite.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlmysql.so
                DESTINATION bin/sqldrivers)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
                DESTINATION bin/platforminputcontexts)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqlinuxfb.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqminimal.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqxcb.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/platforms/libqoffscreen.so
                DESTINATION bin/platforms)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevmouseplugin.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevtouchplugin.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevkeyboardplugin.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/generic/libqevdevtabletplugin.so
                DESTINATION bin/generic)

        else()

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCLucene.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCLucene.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCLucene.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCLucene.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCore.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCore.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCore.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtCore.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtDBus.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtDBus.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtDBus.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtDBus.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtGui.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtGui.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtGui.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtGui.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtHelp.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtHelp.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtHelp.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtHelp.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtNetwork.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtNetwork.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtNetwork.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtNetwork.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtOpenGL.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtOpenGL.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtOpenGL.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtOpenGL.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSql.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSql.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSql.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSql.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSvg.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSvg.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSvg.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtSvg.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtTest.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtTest.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtTest.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtTest.so.4.8.6
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtXml.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtXml.so.4
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtXml.so.4.8
                $ENV{CMAKE_PREFIX_PATH}/lib/libQtXml.so.4.8.6
                DESTINATION lib)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/accessible/libqtaccessiblewidgets.so
                DESTINATION bin/accessible)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqconnmanbearer.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqgenericbearer.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/bearer/libqnmbearer.so
                DESTINATION bin/bearer)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/codecs/libqcncodecs.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/codecs/libqjpcodecs.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/codecs/libqjpcodecs.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/codecs/libqjpcodecs.so
                DESTINATION bin/codecs)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/graphicssystems/libqglgraphicssystem.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/graphicssystems/libqtracegraphicssystem.so
                DESTINATION bin/graphicssystems)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/iconengines/libqsvgicon.so
                DESTINATION bin/iconengines)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqgif.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqico.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqjpeg.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqmng.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqsvg.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqtga.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/imageformats/libqtiff.so
                DESTINATION bin/imageformats)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/inputmethods/libqimsw-multi.so
                DESTINATION bin/inputmethods)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlite.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlmysql.so
                $ENV{CMAKE_PREFIX_PATH}/plugins/sqldrivers/libqsqlmysql.so
                DESTINATION bin/sqldrivers)

        endif()
        
    endif (WIN32)

endif (djvPackageThirdParty)


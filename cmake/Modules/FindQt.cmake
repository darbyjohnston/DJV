# Find the Qt library.
#
# This module defines the following variables:
#
# * QT_FOUND
# * QT_INCLUDE_DIRS
# * QT_LIBRARIES
#
# This module defines the following imported targets:
#
# * Qt5::QtCLucene
# * Qt5::QtConcurrent
# * Qt5::QtCore
# * Qt5::QtDBus
# * Qt5::QtGui
# * Qt5::QtHelp
# * Qt5::QtNetwork
# * Qt5::QtOpenGL
# * Qt5::QtOpenGLExtensions
# * Qt5::QtPlatformHeaders
# * Qt5::QtPlatformSupport
# * Qt5::QtSql
# * Qt5::QtSvg
# * Qt5::QtTest
# * Qt5::QtUiPlugin
# * Qt5::QtUiTools
# * Qt5::QtWaylandClient
# * Qt5::QtWidgets
# * Qt5::QtXml
#
# This module defines the following interfaces:
#
# * Qt5

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

set(QT_INCLUDE_DIRS
    ${CMAKE_INSTALL_PREFIX}/include/QtCLucene
    ${CMAKE_INSTALL_PREFIX}/include/QtConcurrent
    ${CMAKE_INSTALL_PREFIX}/include/QtCore
    ${CMAKE_INSTALL_PREFIX}/include/QtDBus
    ${CMAKE_INSTALL_PREFIX}/include/QtGui
    ${CMAKE_INSTALL_PREFIX}/include/QtHelp
    ${CMAKE_INSTALL_PREFIX}/include/QtNetwork
    ${CMAKE_INSTALL_PREFIX}/include/QtOpenGL
    ${CMAKE_INSTALL_PREFIX}/include/QtOpenGLExtensions
    ${CMAKE_INSTALL_PREFIX}/include/QtPlatformHeaders
    ${CMAKE_INSTALL_PREFIX}/include/QtPlatformSupport
    ${CMAKE_INSTALL_PREFIX}/include/QtSql
    ${CMAKE_INSTALL_PREFIX}/include/QtSvg
    ${CMAKE_INSTALL_PREFIX}/include/QtTest
    ${CMAKE_INSTALL_PREFIX}/include/QtUiPlugin
    ${CMAKE_INSTALL_PREFIX}/include/QtUiTools
    ${CMAKE_INSTALL_PREFIX}/include/QtWaylandClient
    ${CMAKE_INSTALL_PREFIX}/include/QtWidgets
    ${CMAKE_INSTALL_PREFIX}/include/QtXml
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(QT_CLUCENE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5CLucened.lib)
        set(QT_CONCURRENT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Concurrentd.lib)
        set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Cored.lib)
        set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5DBusd.lib)
        set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Guid.lib)
        set(QT_HELP_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Helpd.lib)
        set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Networkd.lib)
        set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5OpenGLd.lib)
        set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Sqld.lib)
        set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Svgd.lib)
        set(QT_TEST_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Testd.lib)
        set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Widgetsd.lib)
        set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Xmld.lib)
    else()
        set(QT_CLUCENE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5CLucene.lib)
        set(QT_CONCURRENT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Concurrent.lib)
        set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Core.lib)
        set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5DBus.lib)
        set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Gui.lib)
        set(QT_HELP_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Help.lib)
        set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Network.lib)
        set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5OpenGL.lib)
        set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Sql.lib)
        set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Svg.lib)
        set(QT_TEST_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Test.lib)
        set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Widgets.lib)
        set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Xml.lib)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.dll
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.dll
                DESTINATION bin)
        endif()
    endif()
elseif(APPLE)
    if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(QT_CLUCENE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene_debug.dylib)
        set(QT_CONCURRENT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent_debug.dylib)
        set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core_debug.dylib)
        set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus_debug.dylib)
        set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui_debug.dylib)
        set(QT_HELP_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help_debug.dylib)
        set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network_debug.dylib)
        set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL_debug.dylib)
        set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql_debug.dylib)
        set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg_debug.dylib)
        set(QT_TEST_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test_debug.dylib)
        set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets_debug.dylib)
        set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml_debug.dylib)
    else()
        set(QT_CLUCENE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.dylib)
        set(QT_CONCURRENT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.dylib)
        set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.dylib)
        set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.dylib)
        set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.dylib)
        set(QT_HELP_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.dylib)
        set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.dylib)
        set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.dylib)
        set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.dylib)
        set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.dylib)
        set(QT_TEST_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.dylib)
        set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.dylib)
        set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.dylib)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.5.6.3.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.5.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.5.6.dylib
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.5.6.3.dylib
                DESTINATION lib)
        endif()
    endif()
else()
    if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(QT_CLUCENE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.so)
        set(QT_CONCURRENT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.so)
        set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.so)
        set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.so)
        set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.so)
        set(QT_HELP_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.so)
        set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.so)
        set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.so)
        set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.so)
        set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.so)
        set(QT_TEST_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.so)
        set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.so)
        set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.so)
    else()
        set(QT_CLUCENE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5CLucene.so)
        set(QT_CONCURRENT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Concurrent.so)
        set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.so)
        set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.so)
        set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.so)
        set(QT_HELP_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Help.so)
        set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.so)
        set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.so)
        set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.so)
        set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.so)
        set(QT_TEST_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Test.so)
        set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.so)
        set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.so)
        if(djvThirdPartyPackage)
            install(
                FILES
                ${QT_CLUCENE_LIBRARY}
                ${QT_CLUCENE_LIBRARY}.5
                ${QT_CLUCENE_LIBRARY}.5.6
                ${QT_CLUCENE_LIBRARY}.5.6.3
                ${QT_CONCURRENT_LIBRARY}
                ${QT_CONCURRENT_LIBRARY}.5
                ${QT_CONCURRENT_LIBRARY}.5.6
                ${QT_CONCURRENT_LIBRARY}.5.6.3
                ${QT_CORE_LIBRARY}
                ${QT_CORE_LIBRARY}.5
                ${QT_CORE_LIBRARY}.5.6
                ${QT_CORE_LIBRARY}.5.6.3
                ${QT_DBUS_LIBRARY}
                ${QT_DBUS_LIBRARY}.5
                ${QT_DBUS_LIBRARY}.5.6
                ${QT_DBUS_LIBRARY}.5.6.3
                ${QT_GUI_LIBRARY}
                ${QT_GUI_LIBRARY}.5
                ${QT_GUI_LIBRARY}.5.6
                ${QT_GUI_LIBRARY}.5.6.3
                ${QT_HELP_LIBRARY}
                ${QT_HELP_LIBRARY}.5
                ${QT_HELP_LIBRARY}.5.6
                ${QT_HELP_LIBRARY}.5.6.3
                ${QT_NETWORK_LIBRARY}
                ${QT_NETWORK_LIBRARY}.5
                ${QT_NETWORK_LIBRARY}.5.6
                ${QT_NETWORK_LIBRARY}.5.6.3
                ${QT_OPENGL_LIBRARY}
                ${QT_OPENGL_LIBRARY}.5
                ${QT_OPENGL_LIBRARY}.5.6
                ${QT_OPENGL_LIBRARY}.5.6.3
                ${QT_SQL_LIBRARY}
                ${QT_SQL_LIBRARY}.5
                ${QT_SQL_LIBRARY}.5.6
                ${QT_SQL_LIBRARY}.5.6.3
                ${QT_SVG_LIBRARY}
                ${QT_SVG_LIBRARY}.5
                ${QT_SVG_LIBRARY}.5.6
                ${QT_SVG_LIBRARY}.5.6.3
                ${QT_TEST_LIBRARY}
                ${QT_TEST_LIBRARY}.5
                ${QT_TEST_LIBRARY}.5.6
                ${QT_TEST_LIBRARY}.5.6.3
                ${QT_WIDGETS_LIBRARY}
                ${QT_WIDGETS_LIBRARY}.5
                ${QT_WIDGETS_LIBRARY}.5.6
                ${QT_WIDGETS_LIBRARY}.5.6.3
                ${QT_XML_LIBRARY}
                ${QT_XML_LIBRARY}.5
                ${QT_XML_LIBRARY}.5.6
                ${QT_XML_LIBRARY}.5.6.3
                DESTINATION lib)
        endif()
    endif()
endif()
set(QT_LIBRARIES
    ${QT_CLUCENE_LIBRARY}
    ${QT_CONCURRENT_LIBRARY}
    ${QT_CORE_LIBRARY}
    ${QT_DBUS_LIBRARY}
    ${QT_GUI_LIBRARY}
    ${QT_HELP_LIBRARY}
    ${QT_NETWORK_LIBRARY}
    ${QT_OPENGL_LIBRARY}
    ${QT_SQL_LIBRARY}
    ${QT_SVG_LIBRARY}
    ${QT_TEST_LIBRARY}
    ${QT_WIDGETS_LIBRARY}
    ${QT_XML_LIBRARY}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES})

if(djvThirdPartyPackage)
    install(
        DIRECTORY
        ${CMAKE_INSTALL_PREFIX}/plugins/bearer
        ${CMAKE_INSTALL_PREFIX}/plugins/generic
        ${CMAKE_INSTALL_PREFIX}/plugins/iconengines
        ${CMAKE_INSTALL_PREFIX}/plugins/imageformats
        ${CMAKE_INSTALL_PREFIX}/plugins/platforms
        ${CMAKE_INSTALL_PREFIX}/plugins/sqldrivers
        DESTINATION plugins)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Qt REQUIRED_VARS QT_INCLUDE_DIRS QT_LIBRARIES)

set(QT_MOC ${CMAKE_INSTALL_PREFIX}/bin/moc)
set(QT_RCC ${CMAKE_INSTALL_PREFIX}/bin/rcc)

function(QT5_WRAP_CPP MOC_SOURCE)
    set(MOC_DEFINITIONS)
    get_directory_property(DIR_DEFINITIONS DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)
    foreach(S ${DIR_DEFINITIONS})
        set(MOC_DEFINITIONS ${MOC_DEFINITIONS} -D${S})
    endforeach()
    set(TMP)
    foreach(S ${ARGN})
        get_filename_component(BASE_NAME ${S} NAME_WE)
        set(MOC_OUTPUT moc_${BASE_NAME}.cpp)
        set(TMP ${TMP} ${MOC_OUTPUT})
        add_custom_command(
            OUTPUT ${MOC_OUTPUT}
            COMMAND ${QT_MOC} ${MOC_DEFINITIONS} ${CMAKE_CURRENT_SOURCE_DIR}/${S} -o ${MOC_OUTPUT}
            DEPENDS ${S})
    endforeach()        
    set(${MOC_SOURCE} ${TMP} PARENT_SCOPE)
endfunction()

function(QT5_CREATE_TRANSLATION QM_SOURCE)
endfunction()

# \todo Add the contents of the .qrc file as dependencies.
function(QT5_ADD_RESOURCES RCC_SOURCE)
    set(TMP)
    foreach(S ${ARGN})
        get_filename_component(BASE_NAME ${S} NAME_WE)
        set(RCC_OUTPUT qrc_${BASE_NAME}.cpp)
        set(TMP ${TMP} ${RCC_OUTPUT})
        add_custom_command(
            OUTPUT ${RCC_OUTPUT}
            COMMAND ${QT_RCC} ${CMAKE_CURRENT_SOURCE_DIR}/${S} -name ${BASE_NAME} -o ${RCC_OUTPUT}
            DEPENDS ${S})
    endforeach()        
    set(${RCC_SOURCE} ${TMP} PARENT_SCOPE)
endfunction()

if(QT_FOUND AND NOT TARGET Qt5::QtCLucene)
    add_library(Qt5::QtCLucene UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtCLucene PROPERTIES
        IMPORTED_LOCATION "${QT_CLUCENE_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtConcurrent)
    add_library(Qt5::QtConcurrent UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtConcurrent PROPERTIES
        IMPORTED_LOCATION "${QT_CONCURRENT_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtCore)
    add_library(Qt5::QtCore UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtCore PROPERTIES
        IMPORTED_LOCATION "${QT_CORE_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtDBus)
    add_library(Qt5::QtDBus UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtDBus PROPERTIES
        IMPORTED_LOCATION "${QT_DBUS_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtGui)
    add_library(Qt5::QtGui UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtGui PROPERTIES
        IMPORTED_LOCATION "${QT_GUI_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtHelp)
    add_library(Qt5::QtHelp UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtHelp PROPERTIES
        IMPORTED_LOCATION "${QT_HELP_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtNetwork)
    add_library(Qt5::QtNetwork UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtNetwork PROPERTIES
        IMPORTED_LOCATION "${QT_NETWORK_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtOpenGL)
    add_library(Qt5::QtOpenGL UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtOpenGL PROPERTIES
        IMPORTED_LOCATION "${QT_OPENGL_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtSvg)
    add_library(Qt5::QtSvg UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtSvg PROPERTIES
        IMPORTED_LOCATION "${QT_SVG_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtSql)
    add_library(Qt5::QtSql UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtSql PROPERTIES
        IMPORTED_LOCATION "${QT_SQL_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtTest)
    add_library(Qt5::QtTest UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtTest PROPERTIES
        IMPORTED_LOCATION "${QT_TEST_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtWidgets)
    add_library(Qt5::QtWidgets UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtWidgets PROPERTIES
        IMPORTED_LOCATION "${QT_WIDGETS_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5::QtXml)
    add_library(Qt5::QtXml UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtXml PROPERTIES
        IMPORTED_LOCATION "${QT_XML_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES PNG JPEG ZLIB
        INTERFACE_INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS}")
endif()
if(QT_FOUND AND NOT TARGET Qt5)
    add_library(Qt5 INTERFACE)
    target_link_libraries(Qt5 INTERFACE Qt5::QtCLucene)
    target_link_libraries(Qt5 INTERFACE Qt5::QtConcurrent)
    target_link_libraries(Qt5 INTERFACE Qt5::QtCore)
    target_link_libraries(Qt5 INTERFACE Qt5::QtDBus)
    target_link_libraries(Qt5 INTERFACE Qt5::QtGui)
    target_link_libraries(Qt5 INTERFACE Qt5::QtHelp)
    target_link_libraries(Qt5 INTERFACE Qt5::QtNetwork)
    target_link_libraries(Qt5 INTERFACE Qt5::QtOpenGL)
    target_link_libraries(Qt5 INTERFACE Qt5::QtSql)
    target_link_libraries(Qt5 INTERFACE Qt5::QtSvg)
    target_link_libraries(Qt5 INTERFACE Qt5::QtTest)
    target_link_libraries(Qt5 INTERFACE Qt5::QtWidgets)
    target_link_libraries(Qt5 INTERFACE Qt5::QtXml)
endif()


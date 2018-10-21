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
# * Qt5::QtCore
# * Qt5::QtDBus
# * Qt5::QtGui
# * Qt5::QtNetwork
# * Qt5::QtOpenGL
# * Qt5::QtPrintSupport
# * Qt5::QtSql
# * Qt5::QtSvg
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

find_path(QT_INCLUDE_DIR
    NAMES QtCore/QtCore
    PATH_SUFFIXES qt5)
find_path(QT_CORE_INCLUDE_DIR
    NAMES QtCore
    PATH_SUFFIXES qt5/QtCore
    PATH_SUFFIXES QtCore)
find_path(QT_DBUS_INCLUDE_DIR
    NAMES QtDBus
    PATH_SUFFIXES qt5/QtDBus
    PATH_SUFFIXES QtDBus)
find_path(QT_GUI_INCLUDE_DIR
    NAMES QtGui
    PATH_SUFFIXES qt5/QtGui
    PATH_SUFFIXES QtGui)
find_path(QT_NETWORK_INCLUDE_DIR
    NAMES QtNetwork
    PATH_SUFFIXES qt5/QtNetwork
    PATH_SUFFIXES QtNetwork)
find_path(QT_OPENGL_INCLUDE_DIR
    NAMES QtOpenGL
    PATH_SUFFIXES qt5/QtOpenGL
    PATH_SUFFIXES QtOpenGL)
find_path(QT_INCLUDE_DIR
    NAMES QtPrintSupport/QtPrintSupport
    PATH_SUFFIXES qt5)
find_path(QT_SQL_INCLUDE_DIR
    NAMES QtSql
    PATH_SUFFIXES qt5/QtSql
    PATH_SUFFIXES QtSql)
find_path(QT_SVG_INCLUDE_DIR
    NAMES QtSvg
    PATH_SUFFIXES qt5/QtSvg
    PATH_SUFFIXES QtSvg)
find_path(QT_WIDGETS_INCLUDE_DIR
    NAMES QtWidgets
    PATH_SUFFIXES qt5/QtWidgets
    PATH_SUFFIXES QtWidgets)
find_path(QT_XML_INCLUDE_DIR
    NAMES QtXml
    PATH_SUFFIXES qt5/QtXml
    PATH_SUFFIXES QtXml)
set(QT_INCLUDE_DIRS
    ${QT_INCLUDE_DIR}
    ${QT_CORE_INCLUDE_DIR}
    ${QT_DBUS_INCLUDE_DIR}
    ${QT_GUI_INCLUDE_DIR}
    ${QT_NETWORK_INCLUDE_DIR}
    ${QT_OPENGL_INCLUDE_DIR}
    ${QT_PRINT_SUPPORT_INCLUDE_DIR}
    ${QT_SQL_INCLUDE_DIR}
    ${QT_SVG_INCLUDE_DIR}
    ${QT_WIDGETS_INCLUDE_DIR}
    ${QT_XML_INCLUDE_DIR}
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(NOT djvThirdPartyBuild)
    find_library(QT_CORE_LIBRARY NAMES Qt5Core)
    find_library(QT_DBUS_LIBRARY NAMES Qt5DBus)
    find_library(QT_GUI_LIBRARY NAMES Qt5Gui)
    find_library(QT_NETWORK_LIBRARY NAMES Qt5Network)
    find_library(QT_OPENGL_LIBRARY NAMES Qt5OpenGL)
    find_library(QT_PRINT_SUPPORT_LIBRARY NAMES Qt5PrintSupport)
    find_library(QT_SQL_LIBRARY NAMES Qt5Sql)
    find_library(QT_SVG_LIBRARY NAMES Qt5Svg)
    find_library(QT_WIDGETS_LIBRARY NAMES Qt5Widgets)
    find_library(QT_XML_LIBRARY NAMES Qt5Xml)
else()
    if(WIN32)
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Cored.lib)
            set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5DBusd.lib)
            set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Guid.lib)
            set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Networkd.lib)
            set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5OpenGLd.lib)
            set(QT_PRINT_SUPPORT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5PrintSupportd.lib)
            set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Sqld.lib)
            set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Svgd.lib)
            set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Widgetsd.lib)
            set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Xmld.lib)
        else()
            set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Core.lib)
            set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5DBus.lib)
            set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Gui.lib)
            set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Network.lib)
            set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5OpenGL.lib)
            set(QT_PRINT_SUPPORT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5PrintSupport.lib)
            set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Sql.lib)
            set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Svg.lib)
            set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Widgets.lib)
            set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/Qt5Xml.lib)
            if(djvThirdPartyPackage)
                install(
                    FILES
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Core.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5DBus.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Gui.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Network.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5OpenGL.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5PrintSupport.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Sql.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Svg.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Widgets.dll
                    ${CMAKE_INSTALL_PREFIX}/bin/Qt5Xml.dll
                    DESTINATION bin)
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
        endif()
    elseif(APPLE)
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core_debug.dylib)
            set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus_debug.dylib)
            set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui_debug.dylib)
            set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network_debug.dylib)
            set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL_debug.dylib)
            set(QT_PRINT_SUPPORT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport_debug.dylib)
            set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql_debug.dylib)
            set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg_debug.dylib)
            set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets_debug.dylib)
            set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml_debug.dylib)
        else()
            set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.dylib)
            set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.dylib)
            set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.dylib)
            set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.dylib)
            set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.dylib)
            set(QT_PRINT_SUPPORT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.dylib)
            set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.dylib)
            set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.dylib)
            set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.dylib)
            set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.dylib)
            if(djvThirdPartyPackage)
                install(
                    FILES
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
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.5.6.3.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.5.6.3.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.5.6.3.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.5.6.3.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.5.6.3.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.5.6.3.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.5.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.5.6.dylib
                    ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.5.6.3.dylib
                    DESTINATION lib)
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
        endif()
    else()
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
            set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.so)
            set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.so)
            set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.so)
            set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.so)
            set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.so)
            set(QT_PRINT_SUPPORT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.so)
            set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.so)
            set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.so)
            set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.so)
            set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.so)
        else()
            set(QT_CORE_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Core.so)
            set(QT_DBUS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5DBus.so)
            set(QT_GUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Gui.so)
            set(QT_NETWORK_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Network.so)
            set(QT_OPENGL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5OpenGL.so)
            set(QT_PRINT_SUPPORT_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5PrintSupport.so)
            set(QT_SQL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Sql.so)
            set(QT_SVG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Svg.so)
            set(QT_WIDGETS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Widgets.so)
            set(QT_XML_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libQt5Xml.so)
        endif()
        if(djvThirdPartyPackage)
            install(
                FILES
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
                ${QT_NETWORK_LIBRARY}
                ${QT_NETWORK_LIBRARY}.5
                ${QT_NETWORK_LIBRARY}.5.6
                ${QT_NETWORK_LIBRARY}.5.6.3
                ${QT_OPENGL_LIBRARY}
                ${QT_OPENGL_LIBRARY}.5
                ${QT_OPENGL_LIBRARY}.5.6
                ${QT_OPENGL_LIBRARY}.5.6.3
                ${QT_PRINT_SUPPORT_LIBRARY}
                ${QT_PRINT_SUPPORT_LIBRARY}.5
                ${QT_PRINT_SUPPORT_LIBRARY}.5.6
                ${QT_PRINT_SUPPORT_LIBRARY}.5.6.3
                ${QT_SQL_LIBRARY}
                ${QT_SQL_LIBRARY}.5
                ${QT_SQL_LIBRARY}.5.6
                ${QT_SQL_LIBRARY}.5.6.3
                ${QT_SVG_LIBRARY}
                ${QT_SVG_LIBRARY}.5
                ${QT_SVG_LIBRARY}.5.6
                ${QT_SVG_LIBRARY}.5.6.3
                ${QT_WIDGETS_LIBRARY}
                ${QT_WIDGETS_LIBRARY}.5
                ${QT_WIDGETS_LIBRARY}.5.6
                ${QT_WIDGETS_LIBRARY}.5.6.3
                ${QT_XML_LIBRARY}
                ${QT_XML_LIBRARY}.5
                ${QT_XML_LIBRARY}.5.6
                ${QT_XML_LIBRARY}.5.6.3
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5XcbQpa.so
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5XcbQpa.so.5
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5XcbQpa.so.5.6
                ${CMAKE_INSTALL_PREFIX}/lib/libQt5XcbQpa.so.5.6.3
                DESTINATION lib)
            install(
                DIRECTORY
                ${CMAKE_INSTALL_PREFIX}/plugins/bearer
                ${CMAKE_INSTALL_PREFIX}/plugins/generic
                ${CMAKE_INSTALL_PREFIX}/plugins/iconengines
                ${CMAKE_INSTALL_PREFIX}/plugins/imageformats
                ${CMAKE_INSTALL_PREFIX}/plugins/platforminputcontexts
                ${CMAKE_INSTALL_PREFIX}/plugins/platforms
                ${CMAKE_INSTALL_PREFIX}/plugins/sqldrivers
                ${CMAKE_INSTALL_PREFIX}/plugins/wayland-decoration-client
                ${CMAKE_INSTALL_PREFIX}/plugins/wayland-graphics-integration-client
                ${CMAKE_INSTALL_PREFIX}/plugins/xcbglintegrations
                DESTINATION plugins)
        endif()
    endif()
endif()
set(QT_LIBRARIES
    ${QT_CORE_LIBRARY}
    ${QT_DBUS_LIBRARY}
    ${QT_GUI_LIBRARY}
    ${QT_NETWORK_LIBRARY}
    ${QT_OPENGL_LIBRARY}
    ${QT_PRINT_SUPPORT_LIBRARY}
    ${QT_SQL_LIBRARY}
    ${QT_SVG_LIBRARY}
    ${QT_WIDGETS_LIBRARY}
    ${QT_XML_LIBRARY}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES})

find_program(QT_MOC moc)
find_program(QT_LUPDATE lupdate)
find_program(QT_LCONVERT lconvert)
find_program(QT_RCC rcc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Qt
    REQUIRED_VARS
        QT_INCLUDE_DIRS
        QT_LIBRARIES
        QT_MOC
        QT_LUPDATE
        QT_LCONVERT
        QT_RCC)
mark_as_advanced(
    QT_INCLUDE_DIR
    QT_CORE_INCLUDE_DIR
    QT_DBUS_INCLUDE_DIR
    QT_GUI_INCLUDE_DIR
    QT_NETWORK_INCLUDE_DIR
    QT_OPENGL_INCLUDE_DIR
    QT_PRINT_SUPPORT_INCLUDE_DIR
    QT_SQL_INCLUDE_DIR
    QT_SVG_INCLUDE_DIR
    QT_WIDGETS_INCLUDE_DIR
    QT_XML_INCLUDE_DIR
    QT_MOC
    QT_LUPDATE
    QT_LCONVERT
    QT_RCC)

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
    set(SOURCE_FILES)
    set(TS_FILES)
    set(TMP)
    foreach(S ${ARGN})
        get_filename_component(FILE_EXTENSION ${S} EXT)
        if("${FILE_EXTENSION}" STREQUAL ".cpp")
            set(SOURCE_FILES ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${S})
        elseif("${FILE_EXTENSION}" STREQUAL ".ts")
            set(TS_FILES ${TS_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${S})
        endif()
    endforeach()
    foreach(TS_FILE ${TS_FILES})
        add_custom_command(
            OUTPUT ${TS_FILE}
            COMMAND ${QT_LUPDATE} ${SOURCE_FILES} -ts ${TS_FILE}
            DEPENDS ${SOURCE_FILES})
        get_filename_component(BASE_NAME ${TS_FILE} NAME_WE)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.qm
            COMMAND ${QT_LCONVERT} -i ${TS_FILE} -o ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.qm
            DEPENDS ${TS_FILE})
        set(TMP ${TMP} ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.qm)
        add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/build/translations/${BASE_NAME}.qm
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.qm ${PROJECT_BINARY_DIR}/build/translations
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.qm)
    endforeach()
    set(${QM_SOURCE} ${TMP} PARENT_SCOPE)
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
if(QT_FOUND AND NOT TARGET Qt5::QtPrintSupport)
    add_library(Qt5::QtPrintSupport UNKNOWN IMPORTED)
    set_target_properties(Qt5::QtPrintSupport PROPERTIES
        IMPORTED_LOCATION "${QT_PRINT_SUPPORT_LIBRARY}"
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
    target_link_libraries(Qt5 INTERFACE Qt5::QtCore)
    target_link_libraries(Qt5 INTERFACE Qt5::QtDBus)
    target_link_libraries(Qt5 INTERFACE Qt5::QtGui)
    target_link_libraries(Qt5 INTERFACE Qt5::QtNetwork)
    target_link_libraries(Qt5 INTERFACE Qt5::QtOpenGL)
    target_link_libraries(Qt5 INTERFACE Qt5::QtPrintSupport)
    target_link_libraries(Qt5 INTERFACE Qt5::QtSql)
    target_link_libraries(Qt5 INTERFACE Qt5::QtSvg)
    target_link_libraries(Qt5 INTERFACE Qt5::QtWidgets)
    target_link_libraries(Qt5 INTERFACE Qt5::QtXml)
endif()


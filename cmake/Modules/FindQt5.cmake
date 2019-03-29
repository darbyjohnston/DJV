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
# * Qt5::QtWidgets
# * Qt5::QtXml
#
# This module defines the following interfaces:
#
# * Qt5

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(FreeType REQUIRED)

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
find_path(QT_PLATFORM_HEADERS_INCLUDE_DIR
    NAMES QtPlatformHeaders
    PATH_SUFFIXES qt5/QtPlatformHeaders
    PATH_SUFFIXES QtPlatformHeaders)
find_path(QT_INCLUDE_DIR
    NAMES QtPrintSupport/QtPrintSupport
    PATH_SUFFIXES qt5)
find_path(QT_SQL_INCLUDE_DIR
    NAMES QtSql
    PATH_SUFFIXES qt5/QtSql
    PATH_SUFFIXES QtSql)
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
    ${QT_PLATFORM_HEADERS_INCLUDE_DIR}
    ${QT_PRINT_SUPPORT_INCLUDE_DIR}
    ${QT_SQL_INCLUDE_DIR}
    ${QT_WIDGETS_INCLUDE_DIR}
    ${QT_XML_INCLUDE_DIR}
    ${FreeType_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(QT_CORE_LIBRARY NAMES Qt5Cored Qt5Core_debug Qt5Core)
    find_library(QT_DBUS_LIBRARY NAMES Qt5DBusd Qt5DBus_debug Qt5DBus)
    find_library(QT_GUI_LIBRARY NAMES Qt5Guid Qt5Gui_debug Qt5Gui)
    find_library(QT_NETWORK_LIBRARY NAMES Qt5Networkd Qt5Network_debug Qt5Network)
    find_library(QT_OPENGL_LIBRARY NAMES Qt5OpenGLd Qt5OpenGL_debug Qt5OpenGL)
    find_library(QT_PRINT_SUPPORT_LIBRARY NAMES Qt5PrintSupportd Qt5PrintSupport_debug Qt5PrintSupport)
    find_library(QT_SQL_LIBRARY NAMES Qt5Sqld Qt5Sql_debug Qt5Sql)
    find_library(QT_WIDGETS_LIBRARY NAMES Qt5Widgetsd  Qt5Widgets_debug  Qt5Widgets)
    find_library(QT_XML_LIBRARY NAMES Qt5Xmld Qt5Xml_debug Qt5Xml)
else()
    find_library(QT_CORE_LIBRARY NAMES Qt5Core)
    find_library(QT_DBUS_LIBRARY NAMES Qt5DBus)
    find_library(QT_GUI_LIBRARY NAMES Qt5Gui)
    find_library(QT_NETWORK_LIBRARY NAMES Qt5Network)
    find_library(QT_OPENGL_LIBRARY NAMES Qt5OpenGL)
    find_library(QT_PRINT_SUPPORT_LIBRARY NAMES Qt5PrintSupport)
    find_library(QT_SQL_LIBRARY NAMES Qt5Sql)
    find_library(QT_WIDGETS_LIBRARY NAMES Qt5Widgets)
    find_library(QT_XML_LIBRARY NAMES Qt5Xml)
endif()
set(QT_LIBRARIES
    ${QT_CORE_LIBRARY}
    ${QT_DBUS_LIBRARY}
    ${QT_GUI_LIBRARY}
    ${QT_NETWORK_LIBRARY}
    ${QT_OPENGL_LIBRARY}
    ${QT_PRINT_SUPPORT_LIBRARY}
    ${QT_SQL_LIBRARY}
    ${QT_WIDGETS_LIBRARY}
    ${QT_XML_LIBRARY}
    ${FreeType_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES})

find_program(QT_MOC NAMES moc-qt5 moc)
#find_program(QT_LUPDATE NAMES lupdate-qt5 lupdate)
#find_program(QT_LCONVERT NAMES lconvert-qt5 lconvert)
find_program(QT_RCC NAMES rcc-qt5 rcc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Qt
    REQUIRED_VARS
        QT_INCLUDE_DIRS
        QT_LIBRARIES
        QT_MOC
        #QT_LUPDATE
        #QT_LCONVERT
        QT_RCC)
mark_as_advanced(
    QT_INCLUDE_DIR
    QT_CORE_INCLUDE_DIR
    QT_DBUS_INCLUDE_DIR
    QT_GUI_INCLUDE_DIR
    QT_NETWORK_INCLUDE_DIR
    QT_OPENGL_INCLUDE_DIR
    QT_PLATFORM_HEADERS_INCLUDE_DIR
    QT_PRINT_SUPPORT_INCLUDE_DIR
    QT_SQL_INCLUDE_DIR
    QT_WIDGETS_INCLUDE_DIR
    QT_XML_INCLUDE_DIR
    QT_CORE_LIBRARY
    QT_DBUS_LIBRARY
    QT_GUI_LIBRARY
    QT_NETWORK_LIBRARY
    QT_OPENGL_LIBRARY
    QT_PRINT_SUPPORT_LIBRARY
    QT_SQL_LIBRARY
    QT_WIDGETS_LIBRARY
    QT_XML_LIBRARY
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
    if(QT_LUPDATE AND QT_LCONVERT)
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
    endif()
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
    target_link_libraries(Qt5 INTERFACE Qt5::QtWidgets)
    target_link_libraries(Qt5 INTERFACE Qt5::QtXml)
endif()

if(DJV_THIRD_PARTY)
    if(WIN32)
        install(
            FILES
            ${DJV_THIRD_PARTY}/bin/Qt5Core.dll
            ${DJV_THIRD_PARTY}/bin/Qt5DBus.dll
            ${DJV_THIRD_PARTY}/bin/Qt5Gui.dll
            ${DJV_THIRD_PARTY}/bin/Qt5Network.dll
            ${DJV_THIRD_PARTY}/bin/Qt5OpenGL.dll
            ${DJV_THIRD_PARTY}/bin/Qt5PrintSupport.dll
            ${DJV_THIRD_PARTY}/bin/Qt5Sql.dll
            ${DJV_THIRD_PARTY}/bin/Qt5Widgets.dll
            ${DJV_THIRD_PARTY}/bin/Qt5Xml.dll
            DESTINATION ${DJV_INSTALL_BIN})
        install(
            DIRECTORY
            DESTINATION ${DJV_INSTALL_LIB}/fonts)
        install(
            DIRECTORY
            ${DJV_THIRD_PARTY}/plugins/bearer
            ${DJV_THIRD_PARTY}/plugins/generic
            ${DJV_THIRD_PARTY}/plugins/imageformats
            ${DJV_THIRD_PARTY}/plugins/platforms
            ${DJV_THIRD_PARTY}/plugins/sqldrivers
            DESTINATION ${DJV_INSTALL_PLUGINS})
    elseif(APPLE)
        install(
            FILES
            ${DJV_THIRD_PARTY}/lib/libQt5Core.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Core.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Core.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Core.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5DBus.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5DBus.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5DBus.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5DBus.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Gui.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Gui.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Gui.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Gui.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Network.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Network.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Network.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Network.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5OpenGL.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5OpenGL.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5OpenGL.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5OpenGL.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5PrintSupport.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5PrintSupport.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5PrintSupport.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5PrintSupport.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Sql.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Sql.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Sql.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Sql.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Widgets.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Widgets.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Widgets.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Widgets.5.6.3.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Xml.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Xml.5.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Xml.5.6.dylib
            ${DJV_THIRD_PARTY}/lib/libQt5Xml.5.6.3.dylib
            DESTINATION ${DJV_INSTALL_LIB})
        install(
            DIRECTORY
            DESTINATION ${DJV_INSTALL_LIB}/fonts)
        install(
            DIRECTORY
            ${DJV_THIRD_PARTY}/plugins/bearer
            ${DJV_THIRD_PARTY}/plugins/generic
            ${DJV_THIRD_PARTY}/plugins/imageformats
            ${DJV_THIRD_PARTY}/plugins/platforms
            ${DJV_THIRD_PARTY}/plugins/sqldrivers
            DESTINATION ${DJV_INSTALL_PLUGINS})
    else()
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
            ${QT_WIDGETS_LIBRARY}
            ${QT_WIDGETS_LIBRARY}.5
            ${QT_WIDGETS_LIBRARY}.5.6
            ${QT_WIDGETS_LIBRARY}.5.6.3
            ${QT_XML_LIBRARY}
            ${QT_XML_LIBRARY}.5
            ${QT_XML_LIBRARY}.5.6
            ${QT_XML_LIBRARY}.5.6.3
            ${DJV_THIRD_PARTY}/lib/libQt5XcbQpa.so
            ${DJV_THIRD_PARTY}/lib/libQt5XcbQpa.so.5
            ${DJV_THIRD_PARTY}/lib/libQt5XcbQpa.so.5.6
            ${DJV_THIRD_PARTY}/lib/libQt5XcbQpa.so.5.6.3
            DESTINATION ${DJV_INSTALL_LIB})
        install(
            DIRECTORY
            DESTINATION ${DJV_INSTALL_LIB}/fonts)
        install(
            DIRECTORY
            ${DJV_THIRD_PARTY}/plugins/bearer
            ${DJV_THIRD_PARTY}/plugins/generic
            ${DJV_THIRD_PARTY}/plugins/imageformats
            ${DJV_THIRD_PARTY}/plugins/platforminputcontexts
            ${DJV_THIRD_PARTY}/plugins/platforms
            ${DJV_THIRD_PARTY}/plugins/sqldrivers
            ${DJV_THIRD_PARTY}/plugins/xcbglintegrations
            DESTINATION ${DJV_INSTALL_PLUGINS})
    endif()
endif()


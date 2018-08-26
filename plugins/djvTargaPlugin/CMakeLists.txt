set(header
    djvTarga.h
    djvTargaLoad.h
    djvTargaPlugin.h
    djvTargaSave.h)

set(source
    djvTarga.cpp
    djvTargaLoad.cpp
    djvTargaPlugin.cpp
    djvTargaSave.cpp)

set(widgetHeader
    djvTarga.h
    djvTargaWidget.h)

set(widgetMocHeader
    djvTargaWidget.h)

set(widgetSource
    djvTarga.cpp
    djvTargaWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvTargaPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvTargaPlugin
    ${djvGuiIncludeDirs})

add_library(djvTargaPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvTargaWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvTargaPlugin ${djvCoreLibs})
target_link_libraries(djvTargaWidgetPlugin ${djvGuiLibs})

set_target_properties(djvTargaPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvTargaWidgetPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvTargaPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvTargaWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvTargaPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvTargaWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

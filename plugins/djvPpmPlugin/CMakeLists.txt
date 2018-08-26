set(header
    djvPpm.h
    djvPpmLoad.h
    djvPpmPlugin.h
    djvPpmSave.h)

set(source
    djvPpm.cpp
    djvPpmLoad.cpp
    djvPpmPlugin.cpp
    djvPpmSave.cpp)

set(widgetHeader
    djvPpm.h
    djvPpmWidget.h)

set(widgetMocHeader
    djvPpmWidget.h)

set(widgetSource
    djvPpm.cpp
    djvPpmWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvPpmPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPpmPlugin
    ${djvGuiIncludeDirs})

add_library(djvPpmPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvPpmWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvPpmPlugin ${djvCoreLibs})
target_link_libraries(djvPpmWidgetPlugin ${djvGuiLibs})

set_target_properties(djvPpmPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvPpmWidgetPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvPpmPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvPpmWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvPpmPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvPpmWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

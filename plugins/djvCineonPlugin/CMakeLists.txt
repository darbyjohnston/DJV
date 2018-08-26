set(header
    djvCineon.h
    djvCineonHeader.h
    djvCineonLoad.h
    djvCineonPlugin.h
    djvCineonSave.h)

set(source
    djvCineon.cpp
    djvCineonHeader.cpp
    djvCineonLoad.cpp
    djvCineonPlugin.cpp
    djvCineonSave.cpp)

set(widgetHeader
    djvCineon.h
    djvCineonWidget.h)

set(widgetMocHeader
    djvCineonWidget.h)

set(widgetSource
    djvCineon.cpp
    djvCineonWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvCineonPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin
    ${djvGuiIncludeDirs})

add_library(djvCineonPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvCineonWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvCineonPlugin ${djvCoreLibs})
target_link_libraries(djvCineonWidgetPlugin ${djvGuiLibs})

set_target_properties(djvCineonPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvCineonWidgetPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvCineonPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvCineonWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvCineonPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvCineonWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

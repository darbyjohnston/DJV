set(header
    djvSgi.h
    djvSgiLoad.h
    djvSgiPlugin.h
    djvSgiSave.h)
set(source
    djvSgi.cpp
    djvSgiLoad.cpp
    djvSgiPlugin.cpp
    djvSgiSave.cpp)

set(widgetHeader
    djvSgi.h
    djvSgiWidget.h)
set(widgetMocHeader
    djvSgiWidget.h)
set(widgetSource
    djvSgi.cpp
    djvSgiWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvSgiPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvSgiPlugin
    ${djvGuiIncludeDirs})
add_library(djvSgiPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvSgiWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})
target_link_libraries(djvSgiPlugin ${djvCoreLibs})
target_link_libraries(djvSgiWidgetPlugin ${djvGuiLibs})

set_target_properties(djvSgiPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
set_target_properties(djvSgiWidgetPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvSgiPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvSgiWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvSgiPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvSgiWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

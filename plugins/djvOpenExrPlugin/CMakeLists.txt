set(header
    djvOpenExr.h
    djvOpenExrLoad.h
    djvOpenExrPlugin.h
    djvOpenExrSave.h)
set(source
    djvOpenExr.cpp
    djvOpenExrLoad.cpp
    djvOpenExrPlugin.cpp
    djvOpenExrSave.cpp)

set(widgetHeader
    djvOpenExr.h
    djvOpenExrWidget.h)
set(widgetMocHeader
    djvOpenExrWidget.h)
set(widgetSource
    djvOpenExr.cpp
    djvOpenExrWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvOpenExrPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvOpenExrPlugin
    ${djvGuiIncludeDirs})
add_library(djvOpenExrPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvOpenExrWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})
target_link_libraries(djvOpenExrPlugin OpenEXR ${djvCoreLibs})
target_link_libraries(djvOpenExrWidgetPlugin OpenEXR ${djvGuiLibs})

set_target_properties(djvOpenExrPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
set_target_properties(djvOpenExrWidgetPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvOpenExrPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvOpenExrWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvOpenExrPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvOpenExrWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

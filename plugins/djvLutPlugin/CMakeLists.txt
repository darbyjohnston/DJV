set(header
    djvLut.h
    djvLutLoad.h
    djvLutPlugin.h
    djvLutSave.h)
set(source
    djvLut.cpp
    djvLutLoad.cpp
    djvLutPlugin.cpp
    djvLutSave.cpp)

set(widgetHeader
    djvLut.h
    djvLutWidget.h)
set(widgetMocHeader
    djvLutWidget.h)
set(widgetSource
    djvLut.cpp
    djvLutWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvLutPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLutPlugin
    ${djvGuiIncludeDirs})
add_library(djvLutPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvLutWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})
target_link_libraries(djvLutPlugin ${djvCoreLibs})
target_link_libraries(djvLutWidgetPlugin ${djvGuiLibs})

set_target_properties(djvLutPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
set_target_properties(djvLutWidgetPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvLutPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvLutWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvLutPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvLutWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

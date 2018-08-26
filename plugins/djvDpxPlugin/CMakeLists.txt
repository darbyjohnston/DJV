set(header
    djvDpx.h
    djvDpxHeader.h
    djvDpxLoad.h
    djvDpxPlugin.h
    djvDpxSave.h)

set(source
    djvDpx.cpp
    djvDpxHeader.cpp
    djvDpxLoad.cpp
    djvDpxPlugin.cpp
    djvDpxSave.cpp
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin/djvCineon.cpp
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin/djvCineonHeader.cpp)

set(widgetHeader
    djvDpx.h
    djvDpxWidget.h)

set(widgetMocHeader
    djvDpxWidget.h)

set(widgetSource
    djvDpx.cpp
    djvDpxWidget.cpp
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin/djvCineon.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvDpxPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvDpxPlugin
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin
    ${djvGuiIncludeDirs})

add_library(djvDpxPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvDpxWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvDpxPlugin ${djvCoreLibs})
target_link_libraries(djvDpxWidgetPlugin ${djvGuiLibs})

set_target_properties(djvDpxPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvDpxWidgetPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvDpxPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvDpxWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvDpxPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvDpxWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

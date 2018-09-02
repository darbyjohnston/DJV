set(header
    djvTiff.h
    djvTiffLoad.h
    djvTiffPlugin.h
    djvTiffSave.h)
set(source
    djvTiff.cpp
    djvTiffLoad.cpp
    djvTiffPlugin.cpp
    djvTiffSave.cpp)

set(widgetHeader
    djvTiff.h
    djvTiffWidget.h)
set(widgetMocHeader
    djvTiffWidget.h)
set(widgetSource
    djvTiff.cpp
    djvTiffWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvTiffPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvTiffPlugin
    ${djvGuiIncludeDirs})
add_library(djvTiffPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvTiffWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})
target_link_libraries(djvTiffPlugin TIFF ${djvCoreLibs})
target_link_libraries(djvTiffWidgetPlugin TIFF ${djvGuiLibs})

set_target_properties(djvTiffPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
set_target_properties(djvTiffWidgetPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvTiffPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvTiffWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvTiffPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvTiffWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

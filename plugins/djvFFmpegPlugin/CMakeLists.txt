set(header
    djvFFmpeg.h
    djvFFmpegLoad.h
    djvFFmpegPlugin.h
    djvFFmpegSave.h)
set(source
    djvFFmpeg.cpp
    djvFFmpegLoad.cpp
    djvFFmpegPlugin.cpp
    djvFFmpegSave.cpp)

set(widgetHeader
    djvFFmpeg.h
    djvFFmpegWidget.h)
set(widgetMocHeader
    djvFFmpegWidget.h)
set(widgetSource
    djvFFmpeg.cpp
    djvFFmpegWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvFFmpegPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvFFmpegPlugin
    ${djvGuiIncludeDirs})
add_library(djvFFmpegPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvFFmpegWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})
target_link_libraries(djvFFmpegPlugin FFMPEG ${djvCoreLibs})
target_link_libraries(djvFFmpegWidgetPlugin FFMPEG ${djvGuiLibs})

set_target_properties(djvFFmpegPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
set_target_properties(djvFFmpegWidgetPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvFFmpegPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvFFmpegWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvFFmpegPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvFFmpegWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

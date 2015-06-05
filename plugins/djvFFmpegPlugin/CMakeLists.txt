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

if (Qt5Core_DIR)
    QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
        djvFFmpegPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(widgetMocSource ${widgetMocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
        djvFFmpegPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvFFmpegPlugin
    ${FFMPEG_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvFFmpegPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvFFmpegWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvFFmpegPlugin ${FFMPEG_LIBRARIES} ${djvCoreLibs})
target_link_libraries(djvFFmpegWidgetPlugin ${FFMPEG_LIBRARIES} ${djvGuiLibs})

set_target_properties(djvFFmpegPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvFFmpegWidgetPlugin PROPERTIES FOLDER plugins)

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

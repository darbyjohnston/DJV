set(header
    djvFFmpegLoad.h
    djvFFmpegPlugin.h
    djvFFmpegSave.h
    djvFFmpegUtil.h
    djvFFmpegWidget.h)

set(mocHeader
    djvFFmpegWidget.h)

set(source
    djvFFmpegLoad.cpp
    djvFFmpegPlugin.cpp
    djvFFmpegSave.cpp
    djvFFmpegUtil.cpp
    djvFFmpegWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvFFmpegPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvFFmpegPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvFFmpegPlugin
    ${FFMPEG_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvFFmpegPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvFFmpegPlugin
    ${FFMPEG_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvFFmpegPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvFFmpegPlugin PROPERTIES
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

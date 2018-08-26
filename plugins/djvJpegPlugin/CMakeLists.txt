set(header
    djvJpeg.h
    djvJpegLoad.h
    djvJpegPlugin.h
    djvJpegSave.h)

set(source
    djvJpeg.cpp
    djvJpegLoad.cpp
    djvJpegPlugin.cpp
    djvJpegSave.cpp)

set(widgetHeader
    djvJpeg.h
    djvJpegWidget.h)

set(widgetMocHeader
    djvJpegWidget.h)

set(widgetSource
    djvJpeg.cpp
    djvJpegWidget.cpp)

QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
    djvJpegPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvJpegPlugin
    ${JPEG_INCLUDE_DIR}
    ${djvGuiIncludeDirs})

add_library(djvJpegPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})
add_library(djvJpegWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvJpegPlugin ${JPEG_LIBRARIES} ${djvCoreLibs})
target_link_libraries(djvJpegWidgetPlugin ${JPEG_LIBRARIES} ${djvGuiLibs})

set_target_properties(djvJpegPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvJpegWidgetPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvJpegPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvJpegWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvJpegPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvJpegWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


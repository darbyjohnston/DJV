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

if (Qt5Core_DIR)
    QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
        djvOpenExrPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(widgetMocSource ${widgetMocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
        djvOpenExrPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvOpenExrPlugin
    ${OPENEXR_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvOpenExrPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvOpenExrWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvOpenExrPlugin ${OPENEXR_LIBRARIES} ${ZLIB_LIBRARIES} ${djvCoreLibs})
target_link_libraries(djvOpenExrWidgetPlugin ${OPENEXR_LIBRARIES} ${ZLIB_LIBRARIES} ${djvGuiLibs})

set_target_properties(djvOpenExrPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvOpenExrWidgetPlugin PROPERTIES FOLDER plugins)

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

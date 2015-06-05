set(header
    djvIff.h
    djvIffLoad.h
    djvIffPlugin.h
    djvIffSave.h)

set(source
    djvIff.cpp
    djvIffLoad.cpp
    djvIffPlugin.cpp
    djvIffSave.cpp)

set(widgetHeader
    djvIff.h
    djvIffWidget.h)

set(widgetMocHeader
    djvIffWidget.h)

set(widgetSource
    djvIff.cpp
    djvIffWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(widgetMocSource ${widgetMocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
        djvIffPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(widgetMocSource ${widgetMocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source} ${widgetSource}
        djvIffPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvIffPlugin
    ${djvGuiIncludeDirs})

add_library(djvIffPlugin MODULE ${header} ${source} ${qmSource})
add_library(djvIffWidgetPlugin MODULE ${widgetHeader} ${widgetSource} ${widgetMocSource} ${qmSource})

target_link_libraries(djvIffPlugin ${djvCoreLibs})
target_link_libraries(djvIffWidgetPlugin ${djvGuiLibs})

set_target_properties(djvIffPlugin PROPERTIES FOLDER plugins)
set_target_properties(djvIffWidgetPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvIffPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
    set_target_properties(
        djvIffWidgetPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvIffPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    TARGETS djvIffWidgetPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

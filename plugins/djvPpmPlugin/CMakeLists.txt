set(header
    djvPpmLoad.h
    djvPpmPlugin.h
    djvPpmSave.h
    djvPpmWidget.h)

set(mocHeader
    djvPpmWidget.h)

set(source
    djvPpmLoad.cpp
    djvPpmPlugin.cpp
    djvPpmSave.cpp
    djvPpmWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvPpmPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvPpmPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPpmPlugin
    ${djvGuiIncludeDirs})

add_library(djvPpmPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvPpmPlugin
    ${djvGuiLibs})

set_target_properties(djvPpmPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvPpmPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvPpmPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

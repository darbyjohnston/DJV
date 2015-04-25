set(header
    djvSgiLoad.h
    djvSgiPlugin.h
    djvSgiSave.h
    djvSgiWidget.h)

set(mocHeader
    djvSgiWidget.h)

set(source
    djvSgiLoad.cpp
    djvSgiPlugin.cpp
    djvSgiSave.cpp
    djvSgiWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvSgiPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvSgiPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvSgiPlugin
    ${djvGuiIncludeDirs})

add_library(djvSgiPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvSgiPlugin
    ${djvGuiLibs})

set_target_properties(djvSgiPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvSgiPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvSgiPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

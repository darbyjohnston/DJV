set(header
    djvTargaLoad.h
    djvTargaPlugin.h
    djvTargaSave.h
#    djvTargaWidget.h
    )

set(mocHeader
#    djvTargaWidget.h
    )

set(source
    djvTargaLoad.cpp
    djvTargaPlugin.cpp
    djvTargaSave.cpp
#    djvTargaWidget.cpp
    )

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvTargaPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvTargaPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvTargaPlugin
    ${djvGuiIncludeDirs})

add_library(djvTargaPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvTargaPlugin
    ${djvGuiLibs})

set_target_properties(djvTargaPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvTargaPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvTargaPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

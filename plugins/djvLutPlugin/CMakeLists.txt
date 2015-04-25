set(header
    djvLutLoad.h
    djvLutPlugin.h
    djvLutSave.h
    djvLutWidget.h)

set(mocHeader
    djvLutWidget.h)

set(source
    djvLutLoad.cpp
    djvLutPlugin.cpp
    djvLutSave.cpp
    djvLutWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvLutPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvLutPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLutPlugin
    ${djvGuiIncludeDirs})

add_library(djvLutPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvLutPlugin
    ${djvGuiLibs})

set_target_properties(djvLutPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvLutPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvLutPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

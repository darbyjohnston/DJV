set(header
    djvLibquicktimeLoad.h
    djvLibquicktimePlugin.h
    djvLibquicktimeSave.h
    djvLibquicktimeWidget.h)

set(mocHeader
    djvLibquicktimeWidget.h)

set(source
    djvLibquicktimeLoad.cpp
    djvLibquicktimePlugin.cpp
    djvLibquicktimeSave.cpp
    djvLibquicktimeWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvLibquicktimePlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvLibquicktimePlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLibquicktimePlugin
    ${LIBQUICKTIME_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvLibquicktimePlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvLibquicktimePlugin
    ${LIBQUICKTIME_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvLibquicktimePlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvLibquicktimePlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvLibquicktimePlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

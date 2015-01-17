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
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLibquicktimePlugin
    ${LIBQUICKTIME_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvLibquicktimePlugin MODULE ${header} ${source} ${mocSource})

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
    TARGETS djvLibquicktimePlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

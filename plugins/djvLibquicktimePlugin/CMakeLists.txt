set(header
    djvLibquicktimeLoad.h
    djvLibquicktimePlugin.h
    djvLibquicktimeSave.h
    djvLibquicktimeWidget.h)

set(source
    djvLibquicktimeLoad.cpp
    djvLibquicktimePlugin.cpp
    djvLibquicktimeSave.cpp
    djvLibquicktimeWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLibquicktimePlugin
    ${LIBQUICKTIME_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvLibquicktimePlugin MODULE ${header} ${source})

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

set(header
    djvLutLoad.h
    djvLutPlugin.h
    djvLutSave.h
    djvLutWidget.h)

set(source
    djvLutLoad.cpp
    djvLutPlugin.cpp
    djvLutSave.cpp
    djvLutWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLutPlugin
    ${djvGuiIncludeDirs})

add_library(djvLutPlugin MODULE ${header} ${source})

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
    TARGETS djvLutPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

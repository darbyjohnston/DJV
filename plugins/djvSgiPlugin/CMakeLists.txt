set(header
    djvSgiLoad.h
    djvSgiPlugin.h
    djvSgiSave.h
    djvSgiWidget.h)

set(source
    djvSgiLoad.cpp
    djvSgiPlugin.cpp
    djvSgiSave.cpp
    djvSgiWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvSgiPlugin
    ${djvGuiIncludeDirs})

add_library(djvSgiPlugin MODULE ${header} ${source})

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
    TARGETS djvSgiPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

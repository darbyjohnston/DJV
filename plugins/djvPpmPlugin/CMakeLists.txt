set(header
    djvPpmLoad.h
    djvPpmPlugin.h
    djvPpmSave.h
    djvPpmWidget.h)

set(source
    djvPpmLoad.cpp
    djvPpmPlugin.cpp
    djvPpmSave.cpp
    djvPpmWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPpmPlugin
    ${djvGuiIncludeDirs})

add_library(djvPpmPlugin MODULE ${header} ${source})

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
    TARGETS djvPpmPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

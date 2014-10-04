set(header
    djvTargaLoad.h
    djvTargaPlugin.h
    djvTargaSave.h
    djvTargaWidget.h)

set(source
    djvTargaLoad.cpp
    djvTargaPlugin.cpp
    djvTargaSave.cpp
    djvTargaWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvTargaPlugin
    ${djvGuiIncludeDirs})

add_library(djvTargaPlugin MODULE ${header} ${source})

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
    TARGETS djvTargaPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

set(header
    djvPicLoad.h
    djvPicPlugin.h)

set(source
    djvPicLoad.cpp
    djvPicPlugin.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPicPlugin
    ${djvGuiIncludeDirs})

add_library(djvPicPlugin MODULE ${header} ${source})

target_link_libraries(
    djvPicPlugin
    ${djvGuiLibs})

set_target_properties(djvPicPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvPicPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvPicPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

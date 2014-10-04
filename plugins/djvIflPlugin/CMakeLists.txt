set(header
    djvIflLoad.h
    djvIflPlugin.h)

set(source
    djvIflLoad.cpp
    djvIflPlugin.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvIflPlugin
    ${djvGuiIncludeDirs})

add_library(djvIflPlugin MODULE ${header} ${source})

target_link_libraries(
    djvIflPlugin
    ${djvGuiLibs})

set_target_properties(djvIflPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvIflPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvIflPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

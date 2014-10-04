set(header
    djvCineon.h
    djvCineonHeader.h
    djvCineonLoad.h
    djvCineonPlugin.h
    djvCineonSave.h
    djvCineonWidget.h)

set(source
    djvCineon.cpp
    djvCineonHeader.cpp
    djvCineonLoad.cpp
    djvCineonPlugin.cpp
    djvCineonSave.cpp
    djvCineonWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin
    ${djvGuiIncludeDirs})

add_library(djvCineonPlugin MODULE ${header} ${source})

target_link_libraries(
    djvCineonPlugin
    ${djvGuiLibs})

set_target_properties(djvCineonPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvCineonPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvCineonPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

set(header
    djvDpxHeader.h
    djvDpxLoad.h
    djvDpxPlugin.h
    djvDpxSave.h
    djvDpxWidget.h)

set(source
    djvDpxHeader.cpp
    djvDpxLoad.cpp
    djvDpxPlugin.cpp
    djvDpxSave.cpp
    djvDpxWidget.cpp
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin/djvCineon.cpp
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin/djvCineonHeader.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvDpxPlugin
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin
    ${djvGuiIncludeDirs})

add_library(djvDpxPlugin MODULE ${header} ${source})

target_link_libraries(
    djvDpxPlugin
    ${djvGuiLibs})

set_target_properties(djvDpxPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvDpxPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvDpxPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

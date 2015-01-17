set(header
    djvCineon.h
    djvCineonHeader.h
    djvCineonLoad.h
    djvCineonPlugin.h
    djvCineonSave.h
    djvCineonWidget.h)

set(mocHeader
    djvCineonWidget.h)

set(source
    djvCineon.cpp
    djvCineonHeader.cpp
    djvCineonLoad.cpp
    djvCineonPlugin.cpp
    djvCineonSave.cpp
    djvCineonWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvCineonPlugin
    ${djvGuiIncludeDirs})

add_library(djvCineonPlugin MODULE ${header} ${source} ${mocSource})

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

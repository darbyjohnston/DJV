set(header
    djvLutLoad.h
    djvLutPlugin.h
    djvLutSave.h
    djvLutWidget.h)

set(mocHeader
    djvLutWidget.h)

set(source
    djvLutLoad.cpp
    djvLutPlugin.cpp
    djvLutSave.cpp
    djvLutWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvLutPlugin
    ${djvGuiIncludeDirs})

add_library(djvLutPlugin MODULE ${header} ${source} ${mocSource})

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

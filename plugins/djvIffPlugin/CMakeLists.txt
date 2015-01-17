set(header
    djvIffLoad.h
    djvIffPlugin.h
    djvIffSave.h
    djvIffWidget.h)

set(mocHeader
    djvIffWidget.h)

set(source
    djvIffLoad.cpp
    djvIffPlugin.cpp
    djvIffSave.cpp
    djvIffWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvIffPlugin
    ${djvGuiIncludeDirs})

add_library(djvIffPlugin MODULE ${header} ${source} ${mocSource})

target_link_libraries(
    djvIffPlugin
    ${djvGuiLibs})

set_target_properties(djvIffPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvIffPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvIffPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

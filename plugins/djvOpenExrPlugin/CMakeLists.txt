set(header
    djvOpenExrLoad.h
    djvOpenExrPlugin.h
    djvOpenExrSave.h
    djvOpenExrWidget.h)

set(mocHeader
    djvOpenExrWidget.h)

set(source
    djvOpenExrLoad.cpp
    djvOpenExrPlugin.cpp
    djvOpenExrSave.cpp
    djvOpenExrWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvOpenExrPlugin
    ${OPENEXR_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvOpenExrPlugin MODULE ${header} ${source} ${mocSource})

target_link_libraries(
    djvOpenExrPlugin
    ${OPENEXR_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvOpenExrPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvOpenExrPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvOpenExrPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

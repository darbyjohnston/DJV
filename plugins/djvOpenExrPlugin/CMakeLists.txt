set(header
    djvOpenExrLoad.h
    djvOpenExrPlugin.h
    djvOpenExrSave.h
#    djvOpenExrWidget.h
    )

set(mocHeader
#    djvOpenExrWidget.h
    )

set(source
    djvOpenExrLoad.cpp
    djvOpenExrPlugin.cpp
    djvOpenExrSave.cpp
#    djvOpenExrWidget.cpp
    )


if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvOpenExrPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvOpenExrPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvOpenExrPlugin
    ${OPENEXR_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvOpenExrPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

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
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvOpenExrPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

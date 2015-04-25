set(header
    djvJpegLoad.h
    djvJpegPlugin.h
    djvJpegSave.h
    djvJpegWidget.h)

set(mocHeader
    djvJpegWidget.h)

set(source
    djvJpegLoad.cpp
    djvJpegPlugin.cpp
    djvJpegSave.cpp
    djvJpegWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvJpegPlugin_en_US.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvJpegPlugin_en_US.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvJpegPlugin
    ${JPEG_INCLUDE_DIR}
    ${djvGuiIncludeDirs})

add_library(djvJpegPlugin MODULE ${header} ${source} ${mocSource} ${qmSource})

target_link_libraries(
    djvJpegPlugin
    ${JPEG_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvJpegPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvJpegPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvJpegPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


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
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvJpegPlugin
    ${JPEG_INCLUDE_DIR}
    ${djvGuiIncludeDirs})

add_library(djvJpegPlugin MODULE ${header} ${source} ${mocSource})

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
    TARGETS djvJpegPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


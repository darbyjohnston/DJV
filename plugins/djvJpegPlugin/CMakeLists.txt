set(header
    djvJpegLoad.h
    djvJpegPlugin.h
    djvJpegSave.h
    djvJpegWidget.h)

set(source
    djvJpegLoad.cpp
    djvJpegPlugin.cpp
    djvJpegSave.cpp
    djvJpegWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvJpegPlugin
    ${JPEG_INCLUDE_DIR}
    ${djvGuiIncludeDirs})

add_library(djvJpegPlugin MODULE ${header} ${source})

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


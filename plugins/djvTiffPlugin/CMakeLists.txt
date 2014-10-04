set(header
    djvTiffLoad.h
    djvTiffPlugin.h
    djvTiffSave.h
    djvTiffWidget.h)

set(source
    djvTiffLoad.cpp
    djvTiffPlugin.cpp
    djvTiffSave.cpp
    djvTiffWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvTiffPlugin
    ${TIFF_INCLUDE_DIR}
    ${JPEG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvTiffPlugin MODULE ${header} ${source})

target_link_libraries(
    djvTiffPlugin
    ${TIFF_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvTiffPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvTiffPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvTiffPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

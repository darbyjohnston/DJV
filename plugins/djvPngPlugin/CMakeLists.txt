set(header
    djvPngLoad.h
    djvPngPlugin.h
    djvPngSave.h)

set(source
    djvPngLoad.cpp
    djvPngPlugin.cpp
    djvPngSave.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPngPlugin
    ${PNG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${djvGuiIncludeDirs})

add_library(djvPngPlugin MODULE ${header} ${source})

target_link_libraries(
    djvPngPlugin
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvPngPlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvPngPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvPngPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

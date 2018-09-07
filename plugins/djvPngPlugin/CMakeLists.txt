set(header
    djvPng.h
    djvPngLoad.h
    djvPngPlugin.h
    djvPngSave.h)
set(source
    djvPng.cpp
    djvPngLoad.cpp
    djvPngPlugin.cpp
    djvPngSave.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPngPlugin
    ${djvCoreIncludeDirs})
add_library(djvPngPlugin ${header} ${source})
target_link_libraries(
    djvPngPlugin
    PNG
    ${djvCoreLibs})

set_target_properties(djvPngPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
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

set(header
    djvPic.h
    djvPicLoad.h
    djvPicPlugin.h)
set(source
    djvPic.cpp
    djvPicLoad.cpp
    djvPicPlugin.cpp)

QT5_CREATE_TRANSLATION(qmSource ${source}
    djvPicPlugin_en_US.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvPicPlugin
    ${djvCoreIncludeDirs})
add_library(djvPicPlugin MODULE ${header} ${source} ${qmSource})
target_link_libraries(djvPicPlugin ${djvCoreLibs})

set_target_properties(djvPicPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvPicPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djvPicPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

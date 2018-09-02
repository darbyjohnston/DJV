set(header
    djvIfl.h
    djvIflLoad.h
    djvIflPlugin.h)
set(source
    djvIfl.cpp
    djvIflLoad.cpp
    djvIflPlugin.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvIflPlugin
    ${djvCoreIncludeDirs})
add_library(djvIflPlugin MODULE ${header} ${source})
target_link_libraries(djvIflPlugin ${djvCoreLibs})

set_target_properties(djvIflPlugin PROPERTIES FOLDER plugins CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djvIflPlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    TARGETS djvIflPlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

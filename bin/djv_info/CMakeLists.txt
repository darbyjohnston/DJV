set(header
    djv_info.h)

set(source
    djv_info.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_info
    ${djvCoreIncludeDirs})

add_executable(djv_info ${header} ${source})

target_link_libraries(djv_info ${djvCoreLibs})

set_target_properties(djv_info PROPERTIES FOLDER bin)

if (djvInstallRpath)

    set_target_properties(
        djv_info PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

if (djvImageIoPlugins)

    add_dependencies(djv_info ${djvImageIoPlugins})

endif (djvImageIoPlugins)

install(
    TARGETS djv_info
    RUNTIME DESTINATION bin)

if (UNIX)

    install(
        PROGRAMS djv_info.sh
        DESTINATION bin)

endif (UNIX)


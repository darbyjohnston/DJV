set(header
    djv_ls.h)

set(source
    djv_ls.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_ls
    ${djvCoreIncludeDirs})

add_executable(djv_ls ${header} ${source})

target_link_libraries(djv_ls ${djvCoreLibs})

set_target_properties(djv_ls PROPERTIES FOLDER bin)

if (djvInstallRpath)

    set_target_properties(
        djv_ls PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

if (djvImageIoPlugins)

    add_dependencies(djv_ls ${djvImageIoPlugins})

endif (djvImageIoPlugins)

install(
    TARGETS djv_ls
    RUNTIME DESTINATION bin)

if (UNIX)

    install(
        PROGRAMS djv_ls.sh
        DESTINATION bin)

endif (UNIX)


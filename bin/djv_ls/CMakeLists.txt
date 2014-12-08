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

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_ls_linux.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_ls.sh
        @ONLY)

    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_ls.sh
        DESTINATION bin)

endif (UNIX)


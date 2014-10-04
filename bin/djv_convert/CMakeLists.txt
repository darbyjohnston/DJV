set(header
    djv_convert.h)

set(source
    djv_convert.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_convert
    ${djvCoreIncludeDirs})

add_executable(djv_convert ${header} ${source})

target_link_libraries(djv_convert ${djvCoreLibs})

set_target_properties(djv_convert PROPERTIES FOLDER bin)

if (djvInstallRpath)

    set_target_properties(
        djv_convert PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

if (djvImageIoPlugins)

    add_dependencies(djv_convert ${djvImageIoPlugins})

endif (djvImageIoPlugins)

install(
    TARGETS djv_convert
    RUNTIME DESTINATION bin)

if (UNIX)

    install(
        PROGRAMS djv_convert.sh
        DESTINATION bin)

endif (UNIX)


set(header
    djv_ls.h)

set(mocHeader
    djv_ls.h)

set(source
    djv_ls.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djv_ls_en_US.ts
        djv_ls_cs_CZ.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djv_ls_en_US.ts
        djv_ls_cs_CZ.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_ls
    ${djvCoreIncludeDirs})

add_executable(djv_ls ${header} ${source} ${mocSource} ${qmSource})

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
    FILES ${qmSource}
    DESTINATION translations)

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


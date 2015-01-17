set(header
    djv_info.h)

set(mocHeader
    djv_info.h)

set(source
    djv_info.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djv_info_en_US.ts
        djv_info_cs_CZ.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djv_info_en_US.ts
        djv_info_cs_CZ.ts)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_info
    ${djvCoreIncludeDirs})

add_executable(djv_info ${header} ${source} ${mocSource} ${qmSource})

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
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djv_info
    RUNTIME DESTINATION bin)

if (UNIX)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_info_linux.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_info.sh
        @ONLY)

    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_info.sh
        DESTINATION bin)

endif (UNIX)


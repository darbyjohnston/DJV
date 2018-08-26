set(header
    djvConvertApplication.h
    djvConvertContext.h)

set(mocHeader
    djvConvertApplication.h
    djvConvertContext.h)

set(source
    djvConvertApplication.cpp
    djvConvertContext.cpp
    djvConvertMain.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source}
    djv_convert_en_US.ts
    djv_convert_cs_CZ.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_convert
    ${djvCoreIncludeDirs})

add_executable(djv_convert ${header} ${source} ${mocSource} ${qmSource})

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
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djv_convert
    RUNTIME DESTINATION bin)

if (UNIX)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_convert_linux.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_convert.sh
        @ONLY)

    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_convert.sh
        DESTINATION bin)

endif (UNIX)


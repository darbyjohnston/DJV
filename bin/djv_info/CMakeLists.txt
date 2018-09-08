set(header
    djvInfoApplication.h
    djvInfoContext.h)
set(mocHeader
    djvInfoApplication.h
    djvInfoContext.h)
set(source
    djvInfoApplication.cpp
    djvInfoContext.cpp
    djvInfoMain.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source}
    djv_info_en_US.ts
    djv_info_cs_CZ.ts)

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_info
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djv_info ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_info djvGraphics)
set_target_properties(djv_info PROPERTIES FOLDER bin CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djv_info PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

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


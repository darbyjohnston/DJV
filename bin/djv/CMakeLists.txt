set(header)
set(source main.cpp)
if (WIN32)
    set(header
        ${header}
        winrc.h)
    set(source
        ${source}
        win.rc)
endif()

add_executable(djv WIN32 ${header} ${source})
target_link_libraries(djv djvViewApp)
set_target_properties(
    djv
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)
install(
    TARGETS djv
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})

if (WIN32)
    add_executable(djv. ${header} ${source})
    target_link_libraries(djv. djvViewApp)    
    set_target_properties(
        djv. PROPERTIES
        SUFFIX com
        FOLDER bin
		CXX_STANDARD 11)
    install(
        TARGETS djv.
        RUNTIME DESTINATION ${DJV_INSTALL_BIN})
endif (WIN32)

if(UNIX)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_linux.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv.sh
        @ONLY)
    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv.sh
        DESTINATION ${DJV_INSTALL_BIN})
    if(CPACK_GENERATOR MATCHES "^RPM$" OR CPACK_GENERATOR MATCHES "^DEB$")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${CPACK_PACKAGING_INSTALL_PREFIX}/${DJV_INSTALL_BIN}/djv.sh djv)
        install(
            FILES ${PROJECT_BINARY_DIR}/djv
            DESTINATION /usr/bin)
        configure_file(
            ${PROJECT_SOURCE_DIR}/etc/Linux/djv.desktop.in
            ${PROJECT_BINARY_DIR}/etc/Linux/djv.desktop
            @ONLY)
        install(
            FILES ${PROJECT_BINARY_DIR}/etc/Linux/djv.desktop
            DESTINATION /usr/share/applications)
        install(
            FILES ../../etc/Linux/djv.mime.xml
            DESTINATION /usr/share/mime/packages)
        install(
            FILES ../../etc/Icons/djv-reel-1024.svg
            DESTINATION /usr/share/icons/hicolor/scalable/apps
            RENAME djv.svg)
    endif()
endif()

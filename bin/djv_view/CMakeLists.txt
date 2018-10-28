set(header
	ViewApplication.h)
set(mocHeader
	ViewApplication.h)
set(source
	ViewApplication.cpp)
if (WIN32)
    set(header
        ${header}
        winrc.h)
    set(source
        ${source}
        win.rc)
endif (WIN32)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${OPENGL_INCLUDE_DIRS})
add_executable(djv_view WIN32 ${header} ${source} ${mocSource} ViewMain.cpp)
target_link_libraries(djv_view djvViewLib)
set_target_properties(djv_view PROPERTIES FOLDER bin CXX_STANDARD 11)

install(
    TARGETS djv_view
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})
if (APPLE)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_view_osx.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        @ONLY)
    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        DESTINATION ${DJV_INSTALL_BIN})
elseif (UNIX)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_view_linux.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        @ONLY)
    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        DESTINATION ${DJV_INSTALL_BIN})
    if(CPACK_GENERATOR MATCHES "^RPM$" OR CPACK_GENERATOR MATCHES "^DEB$")
        configure_file(
            ${PROJECT_SOURCE_DIR}/etc/Linux/djv_view.desktop.in
            ${PROJECT_BINARY_DIR}/etc/Linux/djv_view.desktop
            @ONLY)
        install(
            FILES ${PROJECT_BINARY_DIR}/etc/Linux/djv_view.desktop
            DESTINATION /usr/share/applications)
        install(
            FILES ../../etc/Linux/djv_view.mime.xml
            DESTINATION /usr/share/mime/packages)
        install(
            FILES ../../doc/images/logo-filmreel.svg
            DESTINATION /usr/share/icons/hicolor/scalable/apps
            RENAME djv_view.svg)
    endif()
endif (APPLE)

# \todo Build a second binary for a Windows command line version (.com).
if (WIN32)
    add_executable(djv_view. ${header} ${source} ${mocSource} ViewMain.cpp)
    target_link_libraries(djv_view. djvViewLib)    
    set_target_properties(
        djv_view. PROPERTIES
        SUFFIX com
        FOLDER bin)
    install(
        TARGETS djv_view.
        RUNTIME DESTINATION ${DJV_INSTALL_BIN})
endif (WIN32)


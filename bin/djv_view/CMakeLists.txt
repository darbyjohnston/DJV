set(header
	djvViewApplication.h)
set(mocHeader
	djvViewApplication.h)
set(source
	djvViewApplication.cpp)
if (WIN32)
    set(header
        ${header}
        winrc.h)
    set(source
        ${source}
        win.rc)
endif (WIN32)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_view
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${djvGuiIncludeDirs})
add_executable(djv_view WIN32 ${header} ${source} ${mocSource} djvViewMain.cpp)
target_link_libraries(djv_view djvViewLib ${djvGuiLibs})
if (djvImageIoPlugins)
    add_dependencies(djv_view ${djvImageIoPlugins})
endif (djvImageIoPlugins)

set_target_properties(djv_view PROPERTIES FOLDER bin CXX_STANDARD 11)
if (djvInstallRpath)
    set_target_properties(
        djv_view PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")
endif (djvInstallRpath)

install(
    TARGETS djv_view
    RUNTIME DESTINATION bin)
if (APPLE)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_view_osx.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        @ONLY)
    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        DESTINATION bin)
elseif (UNIX)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/djv_view_linux.sh.in
        ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        @ONLY)
    configure_file(
        ${PROJECT_SOURCE_DIR}/etc/Linux/djv_view.desktop.in
        ${PROJECT_BINARY_DIR}/etc/Linux/djv_view.desktop
        @ONLY)
    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/djv_view.sh
        DESTINATION bin)
    install(
        FILES ${PROJECT_BINARY_DIR}/etc/Linux/djv_view.desktop
        DESTINATION /usr/share/applications)
    install(
        FILES ../../etc/Linux/djv_view.mime.xml
        DESTINATION /usr/share/mime/packages)
    install(
        FILES ../../etc/images/djv_view_16x16.png
        DESTINATION /usr/share/icons/hicolor/16x16/apps
        RENAME djv_view.png)
    install(
        FILES ../../etc/images/djv_view_32x32.png
        DESTINATION /usr/share/icons/hicolor/32x32/apps
        RENAME djv_view.png)
    install(
        FILES ../../etc/images/djv_view_48x48.png
        DESTINATION /usr/share/icons/hicolor/48x48/apps
        RENAME djv_view.png)
endif (APPLE)

# \todo Build a second binary for a Windows command line version (.com).
if (WIN32)
    add_executable(djv_view. ${header} ${source} ${mocSource} djvViewMain.cpp)
    target_link_libraries(djv_view. djvViewLib ${djvGuiLibs})
    if (djvImageIoPlugins)
        add_dependencies(djv_view. ${djvImageIoPlugins})
    endif (djvImageIoPlugins)
    
    set_target_properties(
        djv_view. PROPERTIES
        SUFFIX com
        FOLDER bin)
    if (djvInstallRpath)
        set_target_properties(
            djv_view.
            PROPERTIES
            INSTALL_RPATH "${djvInstallRpath}")
    endif (djvInstallRpath)

    install(
        TARGETS djv_view.
        RUNTIME DESTINATION bin)
endif (WIN32)


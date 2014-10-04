set(header)

set(source)

if (WIN32)

    set(header
        ${header}
        ../../lib/djvViewLib/winrc.h)

    set(source
        ${source}
        ../../lib/djvViewLib/win.rc)

endif (WIN32)

include_directories(
    ${CMAKE_SOURCE_DIR}/bin/djv_view
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${djvGuiIncludeDirs})

add_executable(djv_view ${header} ${source} djvViewWinMain.cpp)

target_link_libraries(djv_view djvViewLib ${djvGuiLibs})

set_target_properties(djv_view PROPERTIES FOLDER bin)

if (djvInstallRpath)

    set_target_properties(
        djv_view PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

if (djvImageIoPlugins)

    add_dependencies(djv_view ${djvImageIoPlugins})

endif (djvImageIoPlugins)

install(
    TARGETS djv_view
    RUNTIME DESTINATION bin)

if (UNIX)

    install(
        PROGRAMS djv_view.sh
        DESTINATION bin)

endif (UNIX)

# \todo Build a second binary for a Windows command line version (.com).

if (WIN32)

    add_executable(djv_view. ${header} ${source} djvViewMain.cpp)
  
    target_link_libraries(djv_view. djvViewLib ${djvGuiLibs})
  
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
  
    if (djvImageIoPlugins)
  
        add_dependencies(djv_view. ${djvImageIoPlugins})
  
    endif (djvImageIoPlugins)
  
    install(
        TARGETS djv_view.
        RUNTIME DESTINATION bin)

endif (WIN32)


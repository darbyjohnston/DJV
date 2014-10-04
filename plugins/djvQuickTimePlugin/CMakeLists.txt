set(header
    djvQuickTimeLoad.h
    djvQuickTimePlugin.h
    djvQuickTimeSave.h
    djvQuickTimeWidget.h)

set(source
    djvQuickTimeLoad.cpp
    djvQuickTimePlugin.cpp
    djvQuickTimeSave.cpp
    djvQuickTimeWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvQuickTimePlugin
    ${QUICKTIME_INCLUDE_DIR}
    ${djvGuiIncludeDirs})
    
add_library(djvQuickTimePlugin MODULE ${header} ${source})

target_link_libraries(
    djvQuickTimePlugin
    ${QUICKTIME_LIBRARIES}
    ${djvGuiLibs})

set_target_properties(djvQuickTimePlugin PROPERTIES FOLDER plugins)

if (djvInstallRpath)

    set_target_properties(
        djvQuickTimePlugin PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    TARGETS djvQuickTimePlugin
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

set(header
    djvQuickTimeLoad.h
    djvQuickTimePlugin.h
    djvQuickTimeSave.h
    djvQuickTimeWidget.h)

set(mocHeader
    djvQuickTimeWidget.h)

set(source
    djvQuickTimeLoad.cpp
    djvQuickTimePlugin.cpp
    djvQuickTimeSave.cpp
    djvQuickTimeWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/plugins/djvQuickTimePlugin
    ${QUICKTIME_INCLUDE_DIR}
    ${djvGuiIncludeDirs})
    
add_library(djvQuickTimePlugin MODULE ${header} ${source} ${mocSource})

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

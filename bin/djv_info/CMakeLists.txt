set(header
    InfoApplication.h
    InfoContext.h)
set(mocHeader
    InfoApplication.h
    InfoContext.h)
set(source
    InfoApplication.cpp
    InfoContext.cpp
    InfoMain.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source}
    djv_info_en_US.ts
    djv_info_cs_CZ.ts)

include_directories(${OPENGL_INCLUDE_DIRS})
add_executable(djv_info ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_info djvAV)
set_target_properties(djv_info PROPERTIES FOLDER bin CXX_STANDARD 11)

install(
    FILES ${qmSource}
    DESTINATION ${DJV_INSTALL_TRANSLATIONS})
install(
    TARGETS djv_info
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})


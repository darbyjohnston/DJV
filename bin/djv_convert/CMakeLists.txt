set(header
    ConvertApplication.h
    ConvertContext.h)
set(mocHeader
    ConvertApplication.h
    ConvertContext.h)
set(source
    ConvertApplication.cpp
    ConvertContext.cpp
    ConvertMain.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source}
    djv_convert_en_US.ts
    djv_convert_cs_CZ.ts)

include_directories(${OPENGL_INCLUDE_DIRS})
add_executable(djv_convert ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_convert djvGraphics)
set_target_properties(djv_convert PROPERTIES FOLDER bin CXX_STANDARD 11)

install(
    FILES ${qmSource}
    DESTINATION ${DJV_INSTALL_TRANSLATIONS})
install(
    TARGETS djv_convert
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})


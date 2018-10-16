set(header
    LsApplication.h
    LsContext.h)
set(mocHeader
    LsApplication.h
    LsContext.h)
set(source
    LsApplication.cpp
    LsContext.cpp
    LsMain.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_CREATE_TRANSLATION(qmSource ${source}
    djv_ls_en_US.ts
    djv_ls_cs_CZ.ts)

include_directories(${OPENGL_INCLUDE_DIRS})
add_executable(djv_ls ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_ls djvGraphics)
set_target_properties(djv_ls PROPERTIES FOLDER bin CXX_STANDARD 11)

install(
    FILES ${qmSource}
    DESTINATION translations)
install(
    TARGETS djv_ls
    RUNTIME DESTINATION bin)


set(header
    Application.h
    Context.h
    Load.h
    Playback.h
    PlayBar.h
    View.h
    Window.h)
set(mocHeader
    Application.h
    Context.h
    Load.h
    Playback.h
    PlayBar.h
    View.h
    Window.h)
set(source
    Application.cpp
    Context.cpp
    Load.cpp
    Playback.cpp
    PlayBar.cpp
    View.cpp
    Window.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/experiments/djvImagePlayExperiment
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvImagePlayExperiment ${header} ${source} ${mocSource})
target_link_libraries(djvImagePlayExperiment djvViewLib)
set_target_properties(djvImagePlayExperiment PROPERTIES FOLDER experiments CXX_STANDARD 11)


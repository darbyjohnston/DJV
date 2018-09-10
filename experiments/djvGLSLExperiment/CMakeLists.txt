set(header
    AbstractOp.h
    Application.h
    BlurOp.h
    ColorOp.h
    Context.h
    EdgeOp.h
    ExposureOp.h
    ImageLoad.h
    LevelsOp.h
    MainWindow.h
    NullOp.h
    OpDockWidget.h
    OpListDockWidget.h
    OpsManager.h
    Playback.h
    PlaybackToolBar.h
    ScaleOp.h
    SharpenOp.h
    Util.h
    ViewWidget.h)
set(mocHeader
    AbstractOp.h
    Application.h
    BlurOp.h
    ColorOp.h
    Context.h
    EdgeOp.h
    ExposureOp.h
    ImageLoad.h
    LevelsOp.h
    MainWindow.h
    NullOp.h
    OpDockWidget.h
    OpListDockWidget.h
    OpsManager.h
    Playback.h
    PlaybackToolBar.h
    ScaleOp.h
    SharpenOp.h
    ViewWidget.h)
set(source
    AbstractOp.cpp
    Application.cpp
    BlurOp.cpp
    ColorOp.cpp
    Context.cpp
    EdgeOp.cpp
    ExposureOp.cpp
    ImageLoad.cpp
    LevelsOp.cpp
    MainWindow.cpp
    NullOp.cpp
    OpDockWidget.cpp
    OpListDockWidget.cpp
    OpsManager.cpp
    Playback.cpp
    PlaybackToolBar.cpp
    ScaleOp.cpp
    SharpenOp.cpp
    Util.cpp
    ViewWidget.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/experiments/djvGLSLExperiment
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvGLSLExperiment ${header} ${source} ${mocSource})
target_link_libraries(djvGLSLExperiment djvViewLib)
set_target_properties(djvGLSLExperiment PROPERTIES FOLDER experiments CXX_STANDARD 11)


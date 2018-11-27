find_package(GLM REQUIRED)

set(header
    Context.h
    ContextInline.h
    FileUtil.h
    ICommand.h
    ISystem.h
    UndoStack.h
    Util.h)
set(mocHeader
    Context.h
    ISystem.h
    UndoStack.h)
set(source
    Context.cpp
    FileUtil.cpp
    ICommand.cpp
    ISystem.cpp
    Util.cpp
    UndoStack.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_library(djvViewExperimentCore ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(
    djvViewExperimentCore
    Qt5
    GLM
    ${CMAKE_DL_LIBS})
set_target_properties(
    djvViewExperimentCore
    PROPERTIES
    FOLDER experiments/djvViewExperiment
    CXX_STANDARD 11)

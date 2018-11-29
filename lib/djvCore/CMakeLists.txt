find_package(GLM REQUIRED)

set(header
    Context.h
    ContextInline.h
    Error.h
    FileUtil.h
    ICommand.h
    ISystem.h
    MathUtil.h
    MathUtilInline.h
    Memory.h
    Speed.h
    SpeedInline.h
    StringUtil.h
    UndoStack.h
    Util.h)
set(mocHeader
    Context.h
    ISystem.h
    Memory.h
    UndoStack.h)
set(source
    Context.cpp
    Error.cpp
    FileUtil.cpp
    ICommand.cpp
    ISystem.cpp
    MathUtil.cpp
    Memory.cpp
    Speed.cpp
    StringUtil.cpp
    Util.cpp
    UndoStack.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_library(djvCore ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(
    djvCore
    Qt5
    GLM
    ${CMAKE_DL_LIBS})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

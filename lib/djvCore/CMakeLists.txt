find_package(GLM REQUIRED)

set(header
    Context.h
    ContextInline.h
    Core.h
    Enum.h
    Error.h
    FileUtil.h
    ICommand.h
    ISystem.h
    ListObserver.h
    ListObserverInline.h
    MapObserver.h
    MapObserverInline.h
    Math.h
    MathInline.h
    Memory.h
    MemoryInline.h
    Speed.h
    SpeedInline.h
    String.h
    StringInline.h
    UndoStack.h
    ValueObserver.h
    ValueObserverInline.h)
set(source
    Context.cpp
    Core.cpp
    Error.cpp
    FileUtil.cpp
    ICommand.cpp
    ISystem.cpp
    Math.cpp
    Memory.cpp
    Speed.cpp
    String.cpp
    UndoStack.cpp)

add_library(djvCore ${header} ${source})
target_link_libraries(
    djvCore
    GLM
    ${CMAKE_DL_LIBS})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

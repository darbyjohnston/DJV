find_package(GLM REQUIRED)
find_package(PicoJSON REQUIRED)
find_package(Threads REQUIRED)

set(header
    Animation.h
    AnimationInline.h
    BBox.h
    BBoxInline.h
    Cache.h
    CacheInline.h
    Context.h
    ContextInline.h
    Core.h
    DirectoryModel.h
    DirectoryWatcher.h
    DrivesModel.h
    Enum.h
    Event.h
    Error.h
    FileInfo.h
    FileInfoInline.h
    FileIO.h
    FileIOInline.h
    Frame.h
    FrameInline.h
    ICommand.h
    IEventSystem.h
    IObject.h
    IObjectInline.h
    ISystem.h
    ISystemInline.h
    ListObserver.h
    ListObserverInline.h
    LogSystem.h
    MapObserver.h
    MapObserverInline.h
    Math.h
    MathInline.h
    Matrix.h
    MatrixInline.h
    Memory.h
    MemoryInline.h
    OS.h
    Path.h
    PathInline.h
    PicoJSON.h
    PicoJSONInline.h
    Range.h
    RangeInline.h
    Ray.h
    RayInline.h
    RecentFilesModel.h
    ResourceSystem.h
    String.h
    StringInline.h
    TextSystem.h
    Time.h
    TimeInline.h
    Timer.h
    TimerInline.h
    UndoStack.h
    ValueObserver.h
    ValueObserverInline.h
    Vector.h
    VectorInline.h)
set(source
    Animation.cpp
    Context.cpp
    Core.cpp
    DirectoryModel.cpp
    DrivesModel.cpp
    Event.cpp
    Error.cpp
    FileIO.cpp
    FileInfo.cpp
    Frame.cpp
    ICommand.cpp
    IEventSystem.cpp
    IObject.cpp
    ISystem.cpp
    LogSystem.cpp
    Math.cpp
    Memory.cpp
    OS.cpp
    RecentFilesModel.cpp
    ResourceSystem.cpp
    Path.cpp
    PicoJSON.cpp
    String.cpp
    TextSystem.cpp
    Time.cpp
    Timer.cpp
    UndoStack.cpp)
if (WIN32)
    set(source
        ${source}
        DirectoryWatcherWin32.cpp
        DrivesModelWin32.cpp
        ErrorWin32.cpp
        FileIOWin32.cpp
        FileInfoWin32.cpp
        OSWin32.cpp
        PathWin32.cpp
        ResourceSystemWin32.cpp)
else()
    set(source
        ${source}
        DirectoryWatcherUnix.cpp
        DrivesModelUnix.cpp
        FileIOUnix.cpp
        FileInfoUnix.cpp
        OSUnix.cpp
        PathUnix.cpp
        ResourceSystemUnix.cpp)
endif()

add_library(djvCore ${header} ${source})
set(LIBRARIES
    GLM
    PicoJSON
    Threads::Threads
    ${CMAKE_DL_LIBS})
if (APPLE)
    find_library(CORE_SERVICES CoreServices)
    set(LIBRARIES ${LIBRARIES} ${CORE_SERVICES})
endif()
target_link_libraries(djvCore ${LIBRARIES})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

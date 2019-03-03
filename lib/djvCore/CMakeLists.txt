find_package(FFmpeg REQUIRED)
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
    CoreSystem.h
    DirectoryModel.h
    DirectoryWatcher.h
    DrivesModel.h
    Enum.h
    Event.h
    EventInline.h
    Error.h
    FileInfo.h
    FileInfoInline.h
    FileInfoJSON.h
    FileInfoJSONInline.h
    FileIO.h
    FileIOInline.h
    Frame.h
    FrameInline.h
    ICommand.h
    IEventSystem.h
    INumericValueModel.h
    INumericValueModelInline.h
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
    NumericValueModels.h
    OS.h
    Path.h
    PathInline.h
    PicoJSON.h
    Range.h
    RangeInline.h
    Ray.h
    RayInline.h
    RecentFilesModel.h
    ResourceSystem.h
    Speed.h
    SpeedInline.h
    String.h
    StringInline.h
    TextSystem.h
    Time.h
    TimeInline.h
    Timer.h
    TimerInline.h
    UID.h
    UndoStack.h
    ValueObserver.h
    ValueObserverInline.h
    Vector.h
    VectorInline.h)
set(source
    Animation.cpp
    Context.cpp
    Core.cpp
    CoreSystem.cpp
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
    NumericValueModels.cpp
    OS.cpp
    RecentFilesModel.cpp
    ResourceSystem.cpp
    Path.cpp
    PicoJSON.cpp
    Speed.cpp
    String.cpp
    TextSystem.cpp
    Time.cpp
    Timer.cpp
    UID.cpp
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
    FFMPEG
    GLM
    PicoJSON
    Threads::Threads
    ${CMAKE_DL_LIBS})
if (WIN32)
    set(LIBRARIES ${LIBRARIES} Netapi32.lib mpr.lib)
elseif (APPLE)
    find_library(CORE_SERVICES CoreServices)
    set(LIBRARIES ${LIBRARIES} ${CORE_SERVICES})
endif()
target_link_libraries(djvCore ${LIBRARIES})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

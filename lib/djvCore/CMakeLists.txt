set(header
    Animation.h
    AnimationFunc.h
    BBox.h
    BBoxFunc.h
    BBoxFuncInline.h
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
    EventFunc.h
    ErrorFunc.h
    FileIO.h
    FileIOFunc.h
    FileInfo.h
    FileInfoFunc.h
    FileInfoPrivate.h
    FileSystem.h
    FileSystemFunc.h
    FrameNumber.h
    FrameNumberFunc.h
    ICommand.h
    IEventSystem.h
    INumericValueModel.h
    INumericValueModelInline.h
    IObject.h
    IObjectInline.h
    ISystem.h
    ListObserver.h
    ListObserverInline.h
    LogSystem.h
    MapObserver.h
    MapObserverInline.h
    Math.h
    MathFunc.h
    MathFuncInline.h
    MatrixFunc.h
    MatrixFuncInline.h
    Memory.h
    MemoryFunc.h
    MemoryFuncInline.h
    NumericValueModels.h
    OSFunc.h
    Observer.h
    PathFunc.h
    Path.h
    Range.h
    RangeFunc.h
    RangeFuncInline.h
    RangeInline.h
    RapidJSONFunc.h
    RapidJSONTemplates.h
    RapidJSONTemplatesInline.h
    Rational.h
    RationalFunc.h
    Ray.h
    RayFunc.h
    RayFuncInline.h
    RayInline.h
    RecentFilesModel.h
    ResourceSystem.h
    Speed.h
    SpeedFunc.h
    StringFormat.h
    StringFunc.h
    StringFuncInline.h
    String.h
    TextSystem.h
    Time.h
    TimeFunc.h
    Timer.h
    TimerFunc.h
    UID.h
    UIDFunc.h
    UndoStack.h
    ValueObserver.h
    ValueObserverInline.h
    VectorFunc.h
    VectorFuncInline.h)
set(source
    Animation.cpp
    AnimationFunc.h
    BBoxFunc.cpp
    Context.cpp
    Core.cpp
    CoreSystem.cpp
    DirectoryModel.cpp
    DrivesModel.cpp
    Event.cpp
    EventFunc.cpp
    ErrorFunc.cpp
    FileIO.cpp
    FileIOFunc.cpp
    FileInfoFunc.cpp
    FileInfo.cpp
    FileSystem.cpp
    FrameNumber.cpp
    FrameNumberFunc.cpp
    ICommand.cpp
    IEventSystem.cpp
    IObject.cpp
    ISystem.cpp
    LogSystem.cpp
    MathFunc.cpp
    MemoryFunc.cpp
    NumericValueModels.cpp
    OSFunc.cpp
    RapidJSONFunc.cpp
    Rational.cpp
    RationalFunc.cpp
    RecentFilesModel.cpp
    ResourceSystem.cpp
    PathFunc.cpp
    Path.cpp
    SpeedFunc.cpp
    StringFormat.cpp
    StringFunc.cpp
    TextSystem.cpp
    TimeFunc.cpp
    Timer.cpp
    TimerFunc.cpp
    UIDFunc.cpp
    UndoStack.cpp
    VectorFunc.cpp)
if (WIN32)
    set(source
        ${source}
        DirectoryWatcherWin32.cpp
        DrivesModelWin32.cpp
        ErrorFuncWin32.cpp
        FileIOWin32.cpp
        FileInfoFuncWin32.cpp
        FileInfoWin32.cpp
        FileSystemFuncWin32.cpp
        OSFuncWin32.cpp
        PathFuncWin32.cpp
        PathWin32.cpp
        TimeFuncWin32.cpp)
else()
    set(source
        ${source}
        DirectoryWatcherUnix.cpp
        DrivesModelUnix.cpp
        FileIOUnix.cpp
        FileInfoFuncUnix.cpp
        FileInfoUnix.cpp
        FileSystemFuncUnix.cpp
        OSFuncUnix.cpp
        PathFuncUnix.cpp
        PathUnix.cpp
        TimeFuncUnix.cpp)
endif()

add_library(djvCore ${header} ${source})
set(LIBRARIES
    GLM
    RapidJSON
    FSeq
    Threads::Threads
    ${CMAKE_DL_LIBS})
if (WIN32)
    set(LIBRARIES ${LIBRARIES} Netapi32.lib mpr.lib)
elseif (APPLE)
    find_library(CORE_SERVICES CoreServices)
    set(LIBRARIES ${LIBRARIES} ${CORE_SERVICES})
endif()
if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm")
    set(LIBRARIES ${LIBRARIES} atomic)
endif()
target_link_libraries(djvCore ${LIBRARIES})
set_target_properties(
    djvCore
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvCore
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvCore)


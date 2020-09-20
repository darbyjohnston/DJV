set(header
    Animation.h
    AnimationFunc.h
    AnimationInline.h
    Context.h
    ContextInline.h
    CoreSystem.h
    DirectoryModel.h
    DirectoryWatcher.h
    DrivesModel.h
    Enum.h
    Event.h
    EventFunc.h
    EventInline.h
    File.h
    FileFunc.h
    FileIO.h
    FileIOFunc.h
    FileIOInline.h
    FileInfo.h
    FileInfoFunc.h
    FileInfoInline.h
    FileInfoPrivate.h
    IEventSystem.h
    IObject.h
    IObjectInline.h
    ISystem.h
    ISystemInline.h
    LogSystem.h
    PathFunc.h
    Path.h
    PathInline.h
    RecentFilesModel.h
    ResourceSystem.h
    TextSystem.h
    Timer.h
    TimerInline.h
    TimerFunc.h)
set(source
    Animation.cpp
    AnimationFunc.cpp
    Context.cpp
    CoreSystem.cpp
    DirectoryModel.cpp
    DrivesModel.cpp
    Event.cpp
    EventFunc.cpp
    File.cpp
    FileIO.cpp
    FileIOFunc.cpp
    FileInfoFunc.cpp
    FileInfo.cpp
    IEventSystem.cpp
    IObject.cpp
    ISystem.cpp
    LogSystem.cpp
    PathFunc.cpp
    Path.cpp
    RecentFilesModel.cpp
    ResourceSystem.cpp
    TextSystem.cpp
    Timer.cpp
    TimerFunc.cpp)
if (WIN32)
    set(source
        ${source}
        DirectoryWatcherWin32.cpp
        DrivesModelWin32.cpp
        FileFuncWin32.cpp
        FileIOWin32.cpp
        FileInfoFuncWin32.cpp
        FileInfoWin32.cpp
        PathFuncWin32.cpp
        PathWin32.cpp)
else()
    set(source
        ${source}
        DirectoryWatcherUnix.cpp
        DrivesModelUnix.cpp
        FileFuncUnix.cpp
        FileIOUnix.cpp
        FileInfoFuncUnix.cpp
        FileInfoUnix.cpp
        PathFuncUnix.cpp
        PathUnix.cpp)
endif()

add_library(djvSystem ${header} ${source})
set(LIBRARIES
    djvMath
    djvCore
    FSeq
    Threads::Threads
    ${CMAKE_DL_LIBS})
if (WIN32)
    set(LIBRARIES ${LIBRARIES} Netapi32.lib mpr.lib)
elseif (APPLE)
    find_library(CORE_SERVICES CoreServices)
    set(LIBRARIES ${LIBRARIES} ${CORE_SERVICES})
endif()
target_link_libraries(djvSystem ${LIBRARIES})
set_target_properties(
    djvSystem
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvSystem
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvSystem)


set(header
    Animation.h
    AnimationInline.h
    Context.h
    ContextInline.h
    CoreSystem.h
    DirectoryModel.h
    DirectoryWatcher.h
    DrivesModel.h
    Enum.h
    Event.h
    EventInline.h
    File.h
    FileIO.h
    FileIOInline.h
    FileInfo.h
    FileInfoInline.h
    FileInfoPrivate.h
    IEventSystem.h
    IObject.h
    IObjectInline.h
    ISystem.h
    ISystemInline.h
    LogSystem.h
    Namespace.h
    Path.h
    PathInline.h
    RecentFilesModel.h
    ResourceSystem.h
    TextSystem.h
    Timer.h
    TimerInline.h)
set(source
    Animation.cpp
    Context.cpp
    CoreSystem.cpp
    DirectoryModel.cpp
    DrivesModel.cpp
    Event.cpp
    File.cpp
    FileIO.cpp
    FileInfo.cpp
    IEventSystem.cpp
    IObject.cpp
    ISystem.cpp
    LogSystem.cpp
    Path.cpp
    RecentFilesModel.cpp
    ResourceSystem.cpp
    TextSystem.cpp
    Timer.cpp)
if (WIN32)
    set(source
        ${source}
        DirectoryWatcherWin32.cpp
        DrivesModelWin32.cpp
        FileIOWin32.cpp
        FileInfoWin32.cpp
        FileWin32.cpp
        PathWin32.cpp)
else()
    set(source
        ${source}
        DirectoryWatcherUnix.cpp
        DrivesModelUnix.cpp
        FileIOUnix.cpp
        FileInfoUnix.cpp
        FileUnix.cpp
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


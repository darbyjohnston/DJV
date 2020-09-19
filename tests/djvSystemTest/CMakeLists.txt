set(header
    AnimationTest.h
    AnimationFuncTest.h
	ContextTest.h
    DirectoryModelTest.h
    DirectoryWatcherTest.h
    DrivesModelTest.h
    EventFuncTest.h
    EventTest.h
    FileFuncTest.h
    FileIOFuncTest.h
    FileIOTest.h
    FileInfoFuncTest.h
    FileInfoTest.h
	IEventSystemTest.h
	ISystemTest.h
    LogSystemTest.h
    ObjectTest.h
    PathFuncTest.h
    PathTest.h
	RecentFilesModelTest.h
    TextSystemTest.h
    TimerFuncTest.h
    TimerTest.h)
set(source
    AnimationTest.cpp
    AnimationFuncTest.cpp
	ContextTest.cpp
    DirectoryModelTest.cpp
    DirectoryWatcherTest.cpp
    DrivesModelTest.cpp
    EventFuncTest.cpp
    EventTest.cpp
    FileFuncTest.cpp
    FileIOFuncTest.cpp
    FileIOTest.cpp
    FileInfoFuncTest.cpp
    FileInfoTest.cpp
	IEventSystemTest.cpp
	ISystemTest.cpp
    LogSystemTest.cpp
    ObjectTest.cpp
    PathFuncTest.cpp
    PathTest.cpp
	RecentFilesModelTest.cpp
    TextSystemTest.cpp
    TimerFuncTest.cpp
    TimerTest.cpp)

add_library(djvSystemTest ${header} ${source})
target_link_libraries(djvSystemTest djvTestLib)
set_target_properties(
    djvSystemTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


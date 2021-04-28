set(header
    AnimationTest.h
	ContextTest.h
    DirectoryModelTest.h
    DirectoryWatcherTest.h
    DrivesModelTest.h
    EventTest.h
    FileTest.h
    FileIOTest.h
    FileInfoTest.h
	IEventSystemTest.h
	ISystemTest.h
    LogSystemTest.h
    ObjectTest.h
    PathTest.h
	RecentFilesModelTest.h
    TextSystemTest.h
    TimerTest.h)
set(source
    AnimationTest.cpp
	ContextTest.cpp
    DirectoryModelTest.cpp
    DirectoryWatcherTest.cpp
    DrivesModelTest.cpp
    EventTest.cpp
    FileTest.cpp
    FileIOTest.cpp
    FileInfoTest.cpp
	IEventSystemTest.cpp
	ISystemTest.cpp
    LogSystemTest.cpp
    ObjectTest.cpp
    PathTest.cpp
	RecentFilesModelTest.cpp
    TextSystemTest.cpp
    TimerTest.cpp)

add_library(djvSystemTest ${header} ${source})
target_link_libraries(djvSystemTest djvTestLib)
set_target_properties(
    djvSystemTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


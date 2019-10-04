set(header
    AnimationTest.h
    BBoxTest.h
	CacheTest.h
	ContextTest.h
    DirectoryModelTest.h
    DirectoryWatcherTest.h
    DrivesModelTest.h
    EnumTest.h
    FileIOTest.h
	FrameTest.h
    MathTest.h
    MemoryTest.h
    ObjectTest.h
    PathTest.h
	SpeedTest.h
    StringTest.h
    TimeTest.h)
set(source
    AnimationTest.cpp
    BBoxTest.cpp
	CacheTest.cpp
	ContextTest.cpp
    DirectoryModelTest.cpp
    DirectoryWatcherTest.cpp
    DrivesModelTest.cpp
    EnumTest.cpp
    FileIOTest.cpp
	FrameTest.cpp
    MathTest.cpp
    MemoryTest.cpp
    ObjectTest.cpp
    PathTest.cpp
	SpeedTest.cpp
    StringTest.cpp
    TimeTest.cpp)

add_library(djvCoreTest ${header} ${source})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


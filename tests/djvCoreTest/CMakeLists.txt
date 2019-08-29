set(header
	CacheTest.h
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
	CacheTest.cpp
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


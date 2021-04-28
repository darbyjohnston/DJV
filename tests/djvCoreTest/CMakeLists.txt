set(header
	CacheTest.h
    EnumTest.h
    ErrorTest.h
    ListObserverTest.h
    MapObserverTest.h
    MemoryTest.h
    OSTest.h
	RandomTest.h
	RapidJSONTest.h
    StringFormatTest.h
    StringTest.h
    TimeTest.h
    UIDTest.h
    UndoStackTest.h
    ValueObserverTest.h)
set(source
	CacheTest.cpp
    EnumTest.cpp
    ErrorTest.cpp
    ListObserverTest.cpp
    MapObserverTest.cpp
    MemoryTest.cpp
    OSTest.cpp
	RandomTest.cpp
	RapidJSONTest.cpp
    StringFormatTest.cpp
    StringTest.cpp
    TimeTest.cpp
    UIDTest.cpp
    UndoStackTest.cpp
    ValueObserverTest.cpp)

add_library(djvCoreTest ${header} ${source})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


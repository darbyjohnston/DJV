set(header
	CacheTest.h
    EnumTest.h
    ErrorFuncTest.h
    ListObserverTest.h
    MapObserverTest.h
    MemoryFuncTest.h
    OSFuncTest.h
	RandomFuncTest.h
	RapidJSONFuncTest.h
    StringFormatTest.h
    StringFuncTest.h
    TimeFuncTest.h
    UIDFuncTest.h
    UndoStackTest.h
    ValueObserverTest.h)
set(source
	CacheTest.cpp
    EnumTest.cpp
    ErrorFuncTest.cpp
    ListObserverTest.cpp
    MapObserverTest.cpp
    MemoryFuncTest.cpp
    OSFuncTest.cpp
	RandomFuncTest.cpp
	RapidJSONFuncTest.cpp
    StringFormatTest.cpp
    StringFuncTest.cpp
    TimeFuncTest.cpp
    UIDFuncTest.cpp
    UndoStackTest.cpp
    ValueObserverTest.cpp)

add_library(djvCoreTest ${header} ${source})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


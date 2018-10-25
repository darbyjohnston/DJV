set(header
    BoxTest.h
    BoxUtilTest.h
    CoreContextTest.h
    CoreTest.h
    DebugTest.h
    ErrorTest.h
    FileInfoTest.h
    FileInfoUtilTest.h
    FileIOTest.h
    FileIOUtilTest.h
	ListUtilTest.h
    MathTest.h
    MemoryTest.h
    RangeTest.h
    SequenceTest.h
    SignalBlockerTest.h
    SpeedTest.h
    StringUtilTest.h
    SystemTest.h
    TimeTest.h
    TimerTest.h
    UserTest.h
    VectorUtilTest.h)
set(mocHeader
    SignalBlockerTest.h)
set(source
    BoxTest.cpp
    BoxUtilTest.cpp
    CoreContextTest.cpp
    DebugTest.cpp
    ErrorTest.cpp
    FileInfoTest.cpp
    FileInfoUtilTest.cpp
    FileIOTest.cpp
    FileIOUtilTest.cpp
	ListUtilTest.cpp
    MathTest.cpp
    MemoryTest.cpp
    RangeTest.cpp
    SequenceTest.cpp
    SignalBlockerTest.cpp
    SpeedTest.cpp
    StringUtilTest.cpp
    SystemTest.cpp
    TimeTest.cpp
    TimerTest.cpp
    UserTest.cpp
    VectorUtilTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvCoreTest ${header} ${source} ${mocSource})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(djvCoreTest PROPERTIES FOLDER tests CXX_STANDARD 11)


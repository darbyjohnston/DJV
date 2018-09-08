set(header
    djvBoxTest.h
    djvBoxUtilTest.h
    djvCoreContextTest.h
    djvCoreTest.h
    djvCoreTest.h
    djvDebugTest.h
    djvErrorTest.h
    djvFileInfoTest.h
    djvFileInfoUtilTest.h
    djvFileIOTest.h
    djvFileIOUtilTest.h
	djvListUtilTest.h
    djvMathTest.h
    djvMemoryTest.h
    djvRangeTest.h
    djvSequenceTest.h
    djvSequenceUtilTest.h
    djvSignalBlockerTest.h
    djvSpeedTest.h
    djvStringUtilTest.h
    djvSystemTest.h
    djvTimeTest.h
    djvTimerTest.h
    djvUserTest.h
    djvVectorUtilTest.h)
set(mocHeader
    djvSignalBlockerTest.h)
set(source
    djvBoxTest.cpp
    djvBoxUtilTest.cpp
    djvCoreContextTest.cpp
    djvDebugTest.cpp
    djvErrorTest.cpp
    djvFileInfoTest.cpp
    djvFileInfoUtilTest.cpp
    djvFileIOTest.cpp
    djvFileIOUtilTest.cpp
	djvListUtilTest.cpp
    djvMathTest.cpp
    djvMemoryTest.cpp
    djvRangeTest.cpp
    djvSequenceTest.cpp
    djvSequenceUtilTest.cpp
    djvSignalBlockerTest.cpp
    djvSpeedTest.cpp
    djvStringUtilTest.cpp
    djvSystemTest.cpp
    djvTimeTest.cpp
    djvTimerTest.cpp
    djvUserTest.cpp
    djvVectorUtilTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvCoreTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR})
add_library(djvCoreTest ${header} ${source} ${mocSource})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(djvCoreTest PROPERTIES FOLDER tests CXX_STANDARD 11)


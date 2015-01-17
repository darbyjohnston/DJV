set(header
    djvCoreApplicationTest.h
    djvBoxTest.h
    djvBoxUtilTest.h
    djvColorProfileTest.h
    djvColorTest.h
    djvColorUtilTest.h
    djvDebugTest.h
    djvErrorTest.h
    djvFileInfoTest.h
    djvFileInfoUtilTest.h
    djvFileIoTest.h
    djvFileIoUtilTest.h
    djvImageApplicationTest.h
    djvImageIoTest.h
    djvImageTagsTest.h
    djvImageTest.h
	djvListUtilTest.h
    djvMathTest.h
    djvMatrixTest.h
    djvMatrixUtilTest.h
    djvMemoryBufferTest.h
    djvMemoryTest.h
    djvOpenGlContextTest.h
    djvOpenGlImageTest.h
    djvOpenGlTest.h
    djvPixelDataTest.h
    djvPixelDataUtilTest.h
    djvPixelTest.h
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
    djvVectorTest.h
    djvVectorUtilTest.h)

set(mocHeader
    djvSignalBlockerTest.h)

set(source
    djvCoreApplicationTest.cpp
    djvBoxTest.cpp
    djvBoxUtilTest.cpp
    djvColorProfileTest.cpp
    djvColorTest.cpp
    djvColorUtilTest.cpp
    djvDebugTest.cpp
    djvErrorTest.cpp
    djvFileInfoTest.cpp
    djvFileInfoUtilTest.cpp
    djvFileIoTest.cpp
    djvFileIoUtilTest.cpp
    djvImageApplicationTest.cpp
    djvImageIoTest.cpp
    djvImageTagsTest.cpp
    djvImageTest.cpp
	djvListUtilTest.cpp
    djvMathTest.cpp
    djvMatrixTest.cpp
    djvMatrixUtilTest.cpp
    djvMemoryBufferTest.cpp
    djvMemoryTest.cpp
    djvOpenGlContextTest.cpp
    djvOpenGlImageTest.cpp
    djvOpenGlTest.cpp
    djvPixelDataTest.cpp
    djvPixelDataUtilTest.cpp
    djvPixelTest.cpp
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
    djvVectorTest.cpp
    djvVectorUtilTest.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvCoreTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvCoreIncludeDirs})

add_library(djvCoreTest ${header} ${source} ${mocSource})

target_link_libraries(djvCoreTest djvTestLib ${djvCoreLibs})

set_target_properties(djvCoreTest PROPERTIES FOLDER tests)


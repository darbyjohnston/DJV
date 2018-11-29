set(header
    MathUtilTest.h)
set(mocHeader)
set(source
    MathUtilTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(djvCoreTest ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(
    djvCoreTest
    djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


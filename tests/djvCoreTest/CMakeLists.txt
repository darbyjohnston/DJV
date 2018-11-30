set(header
    MathTest.h)
set(mocHeader)
set(source
    MathTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvCoreTest ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


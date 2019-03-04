set(header
    EnumTest.h
    MathTest.h
    MemoryTest.h
    PathTest.h
    StringTest.h)
set(source
    EnumTest.cpp
    MathTest.cpp
    MemoryTest.cpp
    PathTest.cpp
    StringTest.cpp)

add_library(djvCoreTest ${header} ${source})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


set(header
    OCIOSystemTest.h
    OCIOTest.h)
set(source
    OCIOSystemTest.cpp
    OCIOTest.cpp)

add_library(djvOCIOTest ${header} ${source})
target_link_libraries(djvOCIOTest djvTestLib djvOCIO)
set_target_properties(
    djvOCIOTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


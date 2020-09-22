set(header
    OCIOSystemFuncTest.h
    OCIOSystemTest.h
    OCIOTest.h)
set(source
    OCIOSystemFuncTest.cpp
    OCIOSystemTest.cpp
    OCIOTest.cpp)

add_library(djvOCIOTest ${header} ${source})
target_link_libraries(djvOCIOTest djvTestLib djvOCIO)
set_target_properties(
    djvOCIOTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


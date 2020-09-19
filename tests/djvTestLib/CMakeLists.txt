set(header
    Test.h
    TickTest.h)
set(source
    Test.cpp
    TickTest.cpp)

add_library(djvTestLib ${header} ${source})
target_link_libraries(djvTestLib djvSystem)
set_target_properties(
    djvTestLib
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


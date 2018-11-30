set(header
    Test.h)
set(source
    Test.cpp)

add_library(djvTestLib ${header} ${source})
target_link_libraries(djvTestLib djvCore)
set_target_properties(
    djvTestLib
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


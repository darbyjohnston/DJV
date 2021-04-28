set(header
    EnumTest.h
    FileSystemTest.h)
set(source
    EnumTest.cpp
    FileSystemTest.cpp)

add_library(djvViewAppTest ${header} ${source})
target_link_libraries(djvViewAppTest djvTestLib djvViewApp)
set_target_properties(
    djvViewAppTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

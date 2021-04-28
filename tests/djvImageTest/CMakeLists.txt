set(header
    ColorTest.h
    DataTest.h
    InfoTest.h
    TagsTest.h
    TypeTest.h)
set(source
    ColorTest.cpp
    DataTest.cpp
    InfoTest.cpp
    TagsTest.cpp
    TypeTest.cpp)

add_library(djvImageTest ${header} ${source})
target_link_libraries(djvImageTest djvTestLib djvImage)
set_target_properties(
    djvImageTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


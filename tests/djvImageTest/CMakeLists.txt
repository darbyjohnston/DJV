set(header
    ColorFuncTest.h
    ColorTest.h
    DataFuncTest.h
    DataTest.h
    InfoFuncTest.h
    InfoTest.h
    TagsTest.h
    TypeFuncTest.h
    TypeTest.h)
set(source
    ColorFuncTest.cpp
    ColorTest.cpp
    DataFuncTest.cpp
    DataTest.cpp
    InfoFuncTest.cpp
    InfoTest.cpp
    TagsTest.cpp
    TypeFuncTest.cpp
    TypeTest.cpp)

add_library(djvImageTest ${header} ${source})
target_link_libraries(djvImageTest djvTestLib djvImage)
set_target_properties(
    djvImageTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


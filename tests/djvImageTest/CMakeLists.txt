set(header
    ColorFuncTest.h
    ColorTest.h
    ImageDataFuncTest.h
    ImageDataTest.h
    ImageTest.h
    ImageFuncTest.h
    PixelFuncTest.h
    PixelTest.h
    TagsTest.h)
set(source
    ColorFuncTest.cpp
    ColorTest.cpp
    ImageDataFuncTest.cpp
    ImageDataTest.cpp
    ImageTest.cpp
    ImageFuncTest.cpp
    PixelFuncTest.cpp
    PixelTest.cpp
    TagsTest.cpp)

add_library(djvImageTest ${header} ${source})
target_link_libraries(djvImageTest djvTestLib djvImage)
set_target_properties(
    djvImageTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


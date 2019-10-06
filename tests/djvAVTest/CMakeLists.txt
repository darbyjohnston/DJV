set(header
    AVSystemTest.h
    AudioDataTest.h
    AudioTest.h
    ColorTest.h
    ImageConvertTest.h
    ImageDataTest.h
    ImageTest.h
    PixelTest.h
    TagsTest.h)
set(source
    AVSystemTest.cpp
    AudioDataTest.cpp
    AudioTest.cpp
    ColorTest.cpp
    ImageConvertTest.cpp
    ImageDataTest.cpp
    ImageTest.cpp
    PixelTest.cpp
    TagsTest.cpp)

add_library(djvAVTest ${header} ${source})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

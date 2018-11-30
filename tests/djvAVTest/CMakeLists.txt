set(header
    AudioTest.h
    PixelTest.h)
set(source
    AudioTest.cpp
    PixelTest.cpp)

add_library(djvAVTest ${header} ${source})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

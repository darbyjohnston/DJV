set(header
    AudioDataTest.h
    AudioTest.h
    AVContextTest.h
    AVTest.h
    ColorProfileTest.h
    ColorTest.h
    ColorUtilTest.h
    ImageIOFormatsTest.h
    ImageIOTest.h
    ImageTest.h
    OpenGLImageTest.h
    OpenGLTest.h
    PixelDataTest.h
    PixelDataUtilTest.h
    PixelTest.h
    TagsTest.h)
set(mocHeader)
set(source
    AudioDataTest.cpp
    AudioTest.cpp
    AVContextTest.cpp
    ColorProfileTest.cpp
    ColorTest.cpp
    ColorUtilTest.cpp
    ImageIOFormatsTest.cpp
    ImageIOTest.cpp
    ImageTest.cpp
    OpenGLImageTest.cpp
    OpenGLTest.cpp
    PixelDataTest.cpp
    PixelDataUtilTest.cpp
    PixelTest.cpp
    TagsTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${OPENGL_INCLUDE_DIRS})
add_library(djvAVTest ${header} ${source} ${mocSource})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(djvAVTest PROPERTIES FOLDER tests CXX_STANDARD 11)


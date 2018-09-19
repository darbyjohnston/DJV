set(header
    ColorProfileTest.h
    ColorTest.h
    ColorUtilTest.h
    GraphicsContextTest.h
    GraphicsTest.h
    ImageIOFormatsTest.h
    ImageIOTest.h
    ImageTagsTest.h
    ImageTest.h
    OpenGLImageTest.h
    OpenGLTest.h
    PixelDataTest.h
    PixelDataUtilTest.h
    PixelTest.h)
set(mocHeader)
set(source
    ColorProfileTest.cpp
    ColorTest.cpp
    ColorUtilTest.cpp
    GraphicsContextTest.cpp
    ImageIOFormatsTest.cpp
    ImageIOTest.cpp
    ImageTagsTest.cpp
    ImageTest.cpp
    OpenGLImageTest.cpp
    OpenGLTest.cpp
    PixelDataTest.cpp
    PixelDataUtilTest.cpp
    PixelTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${OPENGL_INCLUDE_DIRS})
add_library(djvGraphicsTest ${header} ${source} ${mocSource})
target_link_libraries(djvGraphicsTest djvTestLib djvGraphics)
set_target_properties(djvGraphicsTest PROPERTIES FOLDER tests CXX_STANDARD 11)


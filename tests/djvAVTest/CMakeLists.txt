set(header
    AVSystemTest.h
    AudioDataTest.h
    AudioSystemTest.h
    AudioTest.h
    CineonTest.h
    ColorTest.h
    DPXTest.h
    EnumTest.h
    FontSystemTest.h
    IOTest.h
    ImageConvertTest.h
    ImageDataTest.h
    ImageTest.h
    OCIOSystemTest.h
    OCIOTest.h
    PixelTest.h
    Render2DTest.h
    Render3DCameraTest.h
    Render3DLightTest.h
    Render3DMaterialTest.h
    ShaderTest.h
    TagsTest.h
    ThumbnailSystemTest.h
    TriangleMeshTest.h)
set(source
    AVSystemTest.cpp
    AudioDataTest.cpp
    AudioSystemTest.cpp
    AudioTest.cpp
    CineonTest.cpp
    ColorTest.cpp
    DPXTest.cpp
    EnumTest.cpp
    FontSystemTest.cpp
    IOTest.cpp
    ImageConvertTest.cpp
    ImageDataTest.cpp
    ImageTest.cpp
    OCIOSystemTest.cpp
    OCIOTest.cpp
    PixelTest.cpp
    Render2DTest.cpp
    Render3DCameraTest.cpp
    Render3DLightTest.cpp
    Render3DMaterialTest.cpp
    ShaderTest.cpp
    TagsTest.cpp
    ThumbnailSystemTest.cpp
    TriangleMeshTest.cpp)

add_library(djvAVTest ${header} ${source})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

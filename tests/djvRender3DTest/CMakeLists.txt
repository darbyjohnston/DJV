set(header
    CameraTest.h
    LightTest.h
    MaterialTest.h
    RenderTest.h)
set(source
    CameraTest.cpp
    LightTest.cpp
    MaterialTest.cpp
    RenderTest.cpp)

add_library(djvRender3DTest ${header} ${source})
target_link_libraries(djvRender3DTest djvTestLib djvRender3D)
set_target_properties(
    djvRender3DTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


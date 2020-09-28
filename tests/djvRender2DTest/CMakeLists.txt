set(header
    DataFuncTest.h
    DataTest.h
    EnumFuncTest.h
    FontSystemTest.h
    RenderSystemTest.h
    RenderTest.h)
set(source
    DataFuncTest.cpp
    DataTest.cpp
    EnumFuncTest.cpp
    FontSystemTest.cpp
    RenderSystemTest.cpp
    RenderTest.cpp)

add_library(djvRender2DTest ${header} ${source})
target_link_libraries(djvRender2DTest djvTestLib djvRender2D)
set_target_properties(
    djvRender2DTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


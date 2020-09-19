set(header
    DataFuncTest.h
    DataTest.h
    EnumFuncTest.h
    FontSystemTest.h
    Render2DSystemTest.h
    RenderTest.h)
set(source
    DataFuncTest.cpp
    DataTest.cpp
    EnumFuncTest.cpp
    FontSystemTest.cpp
    Render2DSystemTest.cpp
    RenderTest.cpp)

add_library(djvRender2DTest ${header} ${source})
target_link_libraries(djvRender2DTest djvTestLib djvRender2D)
set_target_properties(
    djvRender2DTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


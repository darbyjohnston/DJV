set(source Render2DStressTest.cpp)

add_executable(Render2DStressTest ${header} ${source})
target_link_libraries(Render2DStressTest djvRender2D djvCmdLineApp)
set_target_properties(
    Render2DStressTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

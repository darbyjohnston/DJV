set(source Render2DExample.cpp)

add_executable(Render2DExample ${header} ${source})
target_link_libraries(Render2DExample djvCmdLineApp)
set_target_properties(
    Render2DExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

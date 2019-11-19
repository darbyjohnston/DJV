set(source Render3DExample.cpp)

add_executable(Render3DExample ${header} ${source})
target_link_libraries(Render3DExample djvCmdLineApp)
set_target_properties(
    Render3DExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

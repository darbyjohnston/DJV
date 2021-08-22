set(source GOLExample.cpp)

add_executable(GOLExample ${header} ${source})
target_link_libraries(GOLExample djvRender2D djvCmdLineApp)
set_target_properties(
    GOLExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

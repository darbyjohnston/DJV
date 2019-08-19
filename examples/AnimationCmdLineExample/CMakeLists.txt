set(source AnimationCmdLineExample.cpp)

add_executable(AnimationCmdLineExample ${header} ${source})
target_link_libraries(AnimationCmdLineExample djvCmdLineApp)
set_target_properties(
    AnimationCmdLineExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

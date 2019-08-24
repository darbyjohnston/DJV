set(examples
    AnimationCmdLineExample)

foreach(example ${examples})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} djvCmdLineApp)
    set_target_properties(
        ${example}
        PROPERTIES
        FOLDER examples/CmdLine
        CXX_STANDARD 11)
endforeach()


set(examples
    BasicSceneExample)

foreach(example ${examples})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} djvDesktopApp djvScene)
    set_target_properties(
        ${example}
        PROPERTIES
        FOLDER examples/Scene
        CXX_STANDARD 11)
endforeach()


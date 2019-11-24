set(examples
    SceneViewExample)

foreach(example ${examples})
    add_executable(${example} ${example}.h ${example}.cpp)
    target_link_libraries(${example} djvDesktopApp djvUIComponents)
    set_target_properties(
        ${example}
        PROPERTIES
        FOLDER examples/Scene
        CXX_STANDARD 11)
endforeach()

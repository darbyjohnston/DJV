set(examples
    ButtonDesktopExample
    ComboBoxDesktopExample
    HelloWorldDesktopExample
    MDIDesktopExample
    MenuDesktopExample
    SliderDesktopExample
    SplitterDesktopExample)
set(libraries djvDesktopApp)
if(NOT DJV_BUILD_MINIMAL)
    set(examples
        ${examples}
        FileBrowserDesktopExample
        GraphsDesktopExample
        ListWidgetDesktopExample)
    set(libraries ${libraries} djvUIComponents)
endif()

foreach(example ${examples})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} ${libraries})
    set_target_properties(
        ${example}
        PROPERTIES
        FOLDER examples/Desktop
        CXX_STANDARD 11)
endforeach()


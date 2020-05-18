set(examples
    ButtonDesktopExample
    ComboBoxDesktopExample
    FileBrowserDesktopExample
    GraphsDesktopExample
    HelloWorldDesktopExample
	ListWidgetDesktopExample
    MDIDesktopExample
    SliderDesktopExample
    SplitterDesktopExample)

foreach(example ${examples})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} djvDesktopApp djvUIComponents)
    set_target_properties(
        ${example}
        PROPERTIES
        FOLDER examples/Desktop
        CXX_STANDARD 11)
endforeach()


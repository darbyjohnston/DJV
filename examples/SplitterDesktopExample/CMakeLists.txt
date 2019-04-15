set(source SplitterDesktopExample.cpp)

add_executable(SplitterDesktopExample ${header} ${source})
target_link_libraries(SplitterDesktopExample djvDesktopApp)
set_target_properties(
    SplitterDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

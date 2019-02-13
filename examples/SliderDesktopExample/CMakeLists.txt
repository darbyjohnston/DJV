set(source SliderDesktopExample.cpp)

add_executable(SliderDesktopExample ${header} ${source})
target_link_libraries(SliderDesktopExample djvDesktop)
set_target_properties(
    SliderDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

set(source ScrollDesktopExample.cpp)

add_executable(ScrollDesktopExample ${header} ${source})
target_link_libraries(ScrollDesktopExample djvDesktopApp)
set_target_properties(
    ScrollDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

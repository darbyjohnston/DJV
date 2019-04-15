set(source ButtonDesktopExample.cpp)

add_executable(ButtonDesktopExample ${header} ${source})
target_link_libraries(ButtonDesktopExample djvDesktopApp)
set_target_properties(
    ButtonDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

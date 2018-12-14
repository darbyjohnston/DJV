set(source
    HelloWorldDesktopExample.cpp)

add_executable(HelloWorldDesktopExample ${header} ${source})
target_link_libraries(HelloWorldDesktopExample djvDesktop)
set_target_properties(
    HelloWorldDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

set(source
    HelloWorldDesktop.cpp)

add_executable(HelloWorldDesktop ${header} ${source})
target_link_libraries(HelloWorldDesktop djvDesktop)
set_target_properties(
    HelloWorldDesktop
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

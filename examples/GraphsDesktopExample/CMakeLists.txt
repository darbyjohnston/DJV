set(source GraphsDesktopExample.cpp)

add_executable(GraphsDesktopExample ${header} ${source})
target_link_libraries(GraphsDesktopExample djvUIComponents djvDesktopApp)
set_target_properties(
    GraphsDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

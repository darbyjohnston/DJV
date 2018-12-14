set(source MDICanvasDesktopExample.cpp)

add_executable(MDICanvasDesktopExample ${header} ${source})
target_link_libraries(MDICanvasDesktopExample djvDesktop)
set_target_properties(
    MDICanvasDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

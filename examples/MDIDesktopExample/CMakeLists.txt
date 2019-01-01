set(source MDIDesktopExample.cpp)

add_executable(MDIDesktopExample ${header} ${source})
target_link_libraries(MDIDesktopExample djvDesktop)
set_target_properties(
    MDIDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

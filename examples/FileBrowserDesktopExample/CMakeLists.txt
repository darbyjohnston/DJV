set(source FileBrowserDesktopExample.cpp)

add_executable(FileBrowserDesktopExample ${header} ${source})
target_link_libraries(FileBrowserDesktopExample djvDesktop)
set_target_properties(
    FileBrowserDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

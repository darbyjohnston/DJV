set(source
    DesktopWindow.cpp)

add_executable(DesktopWindow ${header} ${source})
target_link_libraries(DesktopWindow djvDesktop)
set_target_properties(
    DesktopWindow
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

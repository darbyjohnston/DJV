set(source ComboBoxDesktopExample.cpp)

add_executable(ComboBoxDesktopExample ${header} ${source})
target_link_libraries(ComboBoxDesktopExample djvDesktopApp)
set_target_properties(
    ComboBoxDesktopExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)

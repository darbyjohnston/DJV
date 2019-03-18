set(header
    Application.h
    Desktop.h
    EventSystem.h
    GLFWSystem.h)
set(source
    Application.cpp
    EventSystem.cpp
    GLFWSystem.cpp)

add_library(djvDesktop ${header} ${source})
target_link_libraries(djvDesktop djvUIComponents)
set_target_properties(
    djvDesktop
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

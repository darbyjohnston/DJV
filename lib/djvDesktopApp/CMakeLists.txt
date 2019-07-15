set(header
    Application.h
    DesktopApp.h
    EventSystem.h
    GLFWSystem.h)
set(source
    Application.cpp
    EventSystem.cpp
    GLFWSystem.cpp)

add_library(djvDesktopApp ${header} ${source})
target_link_libraries(djvDesktopApp djvUI)
set_target_properties(
    djvDesktopApp
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

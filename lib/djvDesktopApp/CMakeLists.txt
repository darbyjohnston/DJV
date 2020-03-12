set(header
    Application.h
    EventSystem.h
    GLFWSystem.h)
set(source
    Application.cpp
    EventSystem.cpp
    GLFWSystem.cpp)

add_library(djvDesktopApp ${header} ${source})
target_link_libraries(djvDesktopApp djvUI djvCmdLineApp)
set_target_properties(
    djvDesktopApp
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

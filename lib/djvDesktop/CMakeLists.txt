set(header
    Application.h
    Desktop.h
    EventSystem.h
    WindowSystem.h)
set(source
    Application.cpp
    EventSystem.cpp
    WindowSystem.cpp)

add_library(djvDesktop ${header} ${source})
target_link_libraries(djvDesktop djvUI)
set_target_properties(
    djvDesktop
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

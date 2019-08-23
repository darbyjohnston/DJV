set(header
    DesktopAppPy.h)
set(source
    Application.cpp
    DesktopAppPy.cpp)

pybind11_add_module(djvDesktopAppPy SHARED ${header} ${source})
target_link_libraries(djvDesktopAppPy PRIVATE djvDesktopApp)
set_target_properties(
    djvDesktopAppPy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

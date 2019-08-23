set(header
    CmdLineAppPy.h)
set(source
    Application.cpp
    CmdLineAppPy.cpp)

pybind11_add_module(djvCmdLineAppPy SHARED ${header} ${source})
target_link_libraries(djvCmdLineAppPy PRIVATE djvCmdLineApp)
set_target_properties(
    djvCmdLineAppPy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

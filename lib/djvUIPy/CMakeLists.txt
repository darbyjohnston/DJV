set(header)
set(source
    Window.cpp)

pybind11_add_module(djvUIPy SHARED ${header} ${source})
target_link_libraries(djvUIPy PRIVATE djvUI)
set_target_properties(
    djvUIPy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

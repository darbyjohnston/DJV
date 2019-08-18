set(header)
set(source
    Context.cpp)

pybind11_add_module(djvCorePy SHARED ${header} ${source})
target_link_libraries(djvCorePy PRIVATE djvCore)
set_target_properties(
    djvCorePy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

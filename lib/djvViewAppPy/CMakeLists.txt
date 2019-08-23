set(header
    ViewAppPy.h)
set(source
	Application.cpp
    ViewAppPy.cpp)

pybind11_add_module(djvViewAppPy SHARED ${header} ${source})
target_link_libraries(djvViewAppPy PRIVATE djvViewApp)
set_target_properties(
    djvViewAppPy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

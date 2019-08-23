set(header
    UIPy.h)
set(source
	Label.cpp
    UIPy.cpp
	Widget.cpp
	Window.cpp)

pybind11_add_module(djvUIPy SHARED ${header} ${source})
target_link_libraries(djvUIPy PRIVATE djvUI)
set_target_properties(
    djvUIPy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

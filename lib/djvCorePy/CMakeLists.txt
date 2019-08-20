set(header
    CorePy.h)
set(source
	Animation.cpp
    BBox.cpp
    Context.cpp
    CorePy.cpp
    FileInfo.cpp
    Frame.cpp
    IObject.cpp
	Path.cpp
    Vector.cpp)

pybind11_add_module(djvCorePy SHARED ${header} ${source})
target_link_libraries(djvCorePy PRIVATE djvCore)
set_target_properties(
    djvCorePy
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

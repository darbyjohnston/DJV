set(header
    ShapeTest.h
    TriangleMeshTest.h
    TriangleMeshFuncTest.h)
set(source
    ShapeTest.cpp
    TriangleMeshTest.cpp
    TriangleMeshFuncTest.cpp)

add_library(djvGeomTest ${header} ${source})
target_link_libraries(djvGeomTest djvTestLib djvGeom)
set_target_properties(
    djvGeomTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


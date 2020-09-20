set(header
    PointList.h
    Shape.h
    TriangleMesh.h
    TriangleMeshInline.h)
set(source
    PointList.cpp
    Shape.cpp
    TriangleMesh.cpp)

add_library(djvGeom ${header} ${source})
set(LIBRARIES
    djvCore
    GLM)
target_link_libraries(djvGeom ${LIBRARIES})
set_target_properties(
    djvGeom
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvGeom
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvGeom)


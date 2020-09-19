set(header
    Camera.h
    CameraInline.h
    Enum.h
    Group.h
    IO.h
    IOInline.h
    IPrimitive.h
    IPrimitiveInline.h
    InstancePrimitive.h
    InstancePrimitiveInline.h
    Layer.h
    LayerInline.h
    Light.h
    LightInline.h
    Material.h
    MaterialInline.h
    MeshPrimitive.h
    MeshPrimitiveInline.h
    NullPrimitive.h
    NullPrimitiveInline.h
    OBJ.h
    PointListPrimitive.h
    PointListPrimitiveInline.h
    PolyLinePrimitive.h
    PolyLinePrimitiveInline.h
    Render.h
    Scene.h
    SceneInline.h
    SceneSystem.h)
set(source
    Camera.cpp
    Enum.cpp
    Group.cpp
    IO.cpp
    IPrimitive.cpp
    InstancePrimitive.cpp
    Layer.cpp
    Light.cpp
    Material.cpp
    MeshPrimitive.cpp
    NullPrimitive.cpp
    OBJ.cpp
    PointListPrimitive.cpp
    PolyLinePrimitive.cpp
    Render.cpp
    Scene.cpp
    SceneSystem.cpp)

if(OpenNURBS_FOUND)
    set(header
        ${header}
        OpenNURBS.h)
    set(source
        ${source}
        OpenNURBS.cpp)
endif()

add_library(djvScene ${header} ${source})
set(LIBRARIES)
if(OpenNURBS_FOUND)
    set(LIBRARIES ${LIBRARIES} OpenNURBS)
endif()
set(LIBRARIES ${LIBRARIES} djvRender3D djvRender2D djvAV)
target_link_libraries(djvScene ${LIBRARIES})
set_target_properties(
    djvScene
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvScene
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvScene)


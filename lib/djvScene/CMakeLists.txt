set(header
    Camera.h
    CameraInline.h
    Enum.h
    Group.h
    IO.h
    IOInline.h
    Layer.h
    LayerInline.h
    Light.h
    LightInline.h
    Material.h
    MaterialInline.h
    OBJ.h
    Primitive.h
    PrimitiveInline.h
    Render.h
    Scene.h
    SceneInline.h
    SceneSystem.h)
set(source
    Camera.cpp
    Enum.cpp
    Group.cpp
    IO.cpp
    Layer.cpp
    Light.cpp
    Material.cpp
    OBJ.cpp
    Primitive.cpp
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
set(LIBRARIES ${LIBRARIES} djvAV)
target_link_libraries(djvScene ${LIBRARIES})
set_target_properties(
    djvScene
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

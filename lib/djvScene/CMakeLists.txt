set(header
    Block.h
    BlockInline.h
    Camera.h
    CameraInline.h
    Enum.h
    Group.h
    Layer.h
    LayerInline.h
    Light.h
    Material.h
    MaterialInline.h
    Primitive.h
    PrimitiveInline.h
    Scene.h
    SceneInline.h)
set(source
    Block.cpp
    Camera.cpp
    Enum.cpp
    Group.cpp
    Layer.cpp
    Light.cpp
    Material.cpp
    Primitive.cpp
    Scene.cpp)

add_library(djvScene ${header} ${source})
target_link_libraries(djvScene djvAV)
set_target_properties(
    djvScene
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

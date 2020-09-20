set(header
    Enum.h
    EnumFunc.h
    GL.h
    GLFWSystem.h
    ImageConvert.h
    MeshCache.h
    Mesh.h
    MeshInline.h
    OffscreenBuffer.h
    OffscreenBufferInline.h
    Shader.h
    ShaderInline.h
    ShaderSystem.h
    TextureAtlas.h
    Texture.h
    TextureInline.h)
set(source
    EnumFunc.cpp
    GLFWSystem.cpp
    ImageConvert.cpp
    MeshCache.cpp
    Mesh.cpp
    OffscreenBuffer.cpp
    Shader.cpp
    ShaderSystem.cpp
    TextureAtlas.cpp
    Texture.cpp)

add_library(djvGL ${header} ${source})
set(LIBRARIES
    djvSystem
    djvImage
    djvGeom
    GLFW
    glad
    OpenGL::GL)
target_link_libraries(djvGL ${LIBRARIES})
set_target_properties(
    djvGL
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvGL
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvGL)


set(header
    Enum.h
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
    Texture.h
    TextureAtlas.h
    TextureInline.h)
set(source
    Enum.cpp
    GLFWSystem.cpp
    ImageConvert.cpp
    MeshCache.cpp
    Mesh.cpp
    OffscreenBuffer.cpp
    Shader.cpp
    ShaderSystem.cpp
    Texture.cpp
    TextureAtlas.cpp)

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


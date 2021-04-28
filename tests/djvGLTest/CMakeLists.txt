set(header
    EnumTest.h
    ImageConvertTest.h
    MeshCacheTest.h
    MeshTest.h
    OffscreenBufferTest.h
    ShaderTest.h
    TextureAtlasTest.h
    TextureTest.h)
set(source
    EnumTest.cpp
    ImageConvertTest.cpp
    MeshCacheTest.cpp
    MeshTest.cpp
    OffscreenBufferTest.cpp
    ShaderTest.cpp
    TextureAtlasTest.cpp
    TextureTest.cpp)

add_library(djvGLTest ${header} ${source})
target_link_libraries(djvGLTest djvTestLib djvGL)
set_target_properties(
    djvGLTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


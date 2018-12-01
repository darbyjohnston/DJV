find_package(FFmpeg REQUIRED)
find_package(IlmBase REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)

set(header
    Audio.h
    AudioData.h
    AudioDataInline.h
    AudioInline.h
    AudioSystem.h
    Context.h
    Color.h
    ColorInline.h
    FFmpeg.h
    IO.h
    IOInline.h
    Image.h
    ImageTags.h
    OpenGL.h
    OpenGLMesh.h
    OpenGLShader.h
    OpenGLTexture.h
    Pixel.h
    PixelData.h
    PixelDataInline.h
    PixelInline.h
    Shader.h
    TriangleMesh.h)
set(source
    Audio.cpp
    AudioData.cpp
    AudioSystem.cpp
    Context.cpp
    Color.cpp
    FFmpeg.cpp
    IO.cpp
    Image.cpp
    ImageTags.cpp
    OpenGLMesh.cpp
    OpenGLShader.cpp
    OpenGLTexture.cpp
    Pixel.cpp
    PixelData.cpp
    Shader.cpp
    TriangleMesh.cpp)

add_library(djvAV ${header} ${source})
target_link_libraries(
    djvAV
    djvCore
    Qt5
    FFMPEG
    IlmBase
    OpenAL
    OpenGL::GL)
set_target_properties(
    djvAV
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

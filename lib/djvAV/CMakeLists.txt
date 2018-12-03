find_package(FreeType REQUIRED)
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
    Color.h
    ColorInline.h
    FFmpeg.h
    FontSystem.h
    IO.h
    IOInline.h
    Image.h
    Keycode.h
    OpenGL.h
    OpenGLMesh.h
    OpenGLShader.h
    OpenGLTexture.h
    Pixel.h
    PixelData.h
    PixelDataInline.h
    PixelInline.h
    Shader.h
    Speed.h
    SpeedInline.h
    System.h
    Tags.h
    Timecode.h
    TimecodeInline.h
    TriangleMesh.h)
set(source
    Audio.cpp
    AudioData.cpp
    AudioSystem.cpp
    Color.cpp
    FFmpeg.cpp
    FontSystem.cpp
    IO.cpp
    Image.cpp
    Keycode.cpp
    OpenGLMesh.cpp
    OpenGLShader.cpp
    OpenGLTexture.cpp
    Pixel.cpp
    PixelData.cpp
    Shader.cpp
    Speed.cpp
    System.cpp
    Tags.cpp
    Timecode.cpp
    TriangleMesh.cpp)

add_library(djvAV ${header} ${source})
target_link_libraries(
    djvAV
    djvCore
    Qt5
    FreeType
    FFMPEG
    IlmBase
    OpenAL
    OpenGL::GL)
set_target_properties(
    djvAV
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

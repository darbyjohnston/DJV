find_package(FreeType REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(IlmBase REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(glbinding REQUIRED)

find_package(PNG)

set(header
    AV.h
    Audio.h
    AudioData.h
    AudioDataInline.h
    AudioInline.h
    AudioSystem.h
    Color.h
    ColorInline.h
    FFmpeg.h
    FFmpegInline.h
    FontSystem.h
    FontSystemInline.h
    IO.h
    IOInline.h
    IconSystem.h
    Image.h
    Keycode.h
    OpenGL.h
    OpenGLMesh.h
    OpenGLMeshInline.h
    OpenGLOffscreenBuffer.h
    OpenGLShader.h
    OpenGLTexture.h
    OpenGLTextureInline.h
    PPM.h
    Pixel.h
    PixelConvert.h
    PixelData.h
    PixelDataInline.h
    PixelInline.h
    Render2DSystem.h
    SequenceIO.h
    Shader.h
    Shape.h
    Speed.h
    SpeedInline.h
    System.h
    Tags.h
    TextureCache.h
    Time.h
    Timecode.h
    TimecodeInline.h
    TriangleMesh.h)
set(source
    Audio.cpp
    AudioData.cpp
    AudioSystem.cpp
    Color.cpp
    FFmpeg.cpp
    FFmpegRead.cpp
    FontSystem.cpp
    IO.cpp
    IconSystem.cpp
    Image.cpp
    Keycode.cpp
    OpenGLMesh.cpp
    OpenGLOffscreenBuffer.cpp
    OpenGLShader.cpp
    OpenGLTexture.cpp
    PPM.cpp
    PPMRead.cpp
    PPMWrite.cpp
    Pixel.cpp
    PixelConvert.cpp
    PixelData.cpp
    Render2DSystem.cpp
    SequenceIO.cpp
    Shape.cpp
    Shader.cpp
    Speed.cpp
    System.cpp
    Tags.cpp
    TextureCache.cpp
    Time.cpp
    Timecode.cpp
    TriangleMesh.cpp)
if(PNG_FOUND)
    set(header
        ${header}
        PNG.h)
    set(source
        ${source}
        PNG.cpp
        PNGRead.cpp
        PNGWrite.cpp)
endif()

add_library(djvAV ${header} ${source})
set(LIBRARIES
    glbinding
    GLFW
    FreeType
    FFMPEG
    IlmBase
    OpenAL
    OpenGL::GL
    djvCore)
if(PNG_FOUND)
    set(LIBRARIES ${LIBRARIES} PNG)
endif()
target_link_libraries(djvAV ${LIBRARIES})
set_target_properties(
    djvAV
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)


find_package(FreeType REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(IlmBase REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)

find_package(PNG)

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
    IconSystem.h
    Image.h
    Keycode.h
    OpenGL.h
    OpenGLMesh.h
    OpenGLOffscreenBuffer.h
    OpenGLShader.h
    OpenGLTexture.h
    PPM.h
    Pixel.h
    PixelData.h
    PixelDataInline.h
    PixelInline.h
    PixelProcess.h
    SequenceIO.h
    Shader.h
    Shape.h
    Speed.h
    SpeedInline.h
    System.h
    Tags.h
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
    PixelData.cpp
    PixelProcess.cpp
    SequenceIO.cpp
    Shape.cpp
    Shader.cpp
    Speed.cpp
    System.cpp
    Tags.cpp
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
set(djvAVLibs
    djvCore
    Qt5
    FreeType
    FFMPEG
    IlmBase
    OpenAL
    OpenGL::GL)
if(PNG_FOUND)
    set(djvAVLibs ${djvAVLibs} PNG)
endif()
target_link_libraries(djvAV ${djvAVLibs})
set_target_properties(
    djvAV
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

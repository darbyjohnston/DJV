find_package(FreeType REQUIRED)
find_package(IlmBase REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(glbinding REQUIRED)

find_package(JPEG)
find_package(PNG)
find_package(TIFF)

set(header
    AV.h
    AVSystem.h
    Application.h
    Audio.h
    AudioData.h
    AudioDataInline.h
    AudioInline.h
    AudioSystem.h
    Color.h
    ColorInline.h
    Enum.h
    FFmpeg.h
    FFmpegInline.h
    FontSystem.h
    FontSystemInline.h
    IO.h
    IOInline.h
    Image.h
    ImageConvert.h
    ImageData.h
    ImageDataInline.h
    OpenGL.h
    OpenGLMesh.h
    OpenGLMeshInline.h
    OpenGLOffscreenBuffer.h
    OpenGLShader.h
    OpenGLTexture.h
    OpenGLTextureInline.h
    PPM.h
    PPMInline.h
    Pixel.h
    PixelInline.h
    Render2D.h
    SequenceIO.h
    Shader.h
    Shape.h
    Tags.h
    TextureAtlas.h
    ThumbnailSystem.h
    TriangleMesh.h)
set(source
    AVSystem.cpp
    Application.cpp
    Audio.cpp
    AudioData.cpp
    AudioSystem.cpp
    Color.cpp
    Enum.cpp
    FFmpeg.cpp
    FFmpegRead.cpp
    FontSystem.cpp
    IO.cpp
    Image.cpp
    ImageConvert.cpp
    ImageData.cpp
    OpenGLMesh.cpp
    OpenGLOffscreenBuffer.cpp
    OpenGLShader.cpp
    OpenGLTexture.cpp
    PPM.cpp
    PPMRead.cpp
    PPMWrite.cpp
    Pixel.cpp
    Render2D.cpp
    SequenceIO.cpp
    Shape.cpp
    Shader.cpp
    Tags.cpp
    TextureAtlas.cpp
    ThumbnailSystem.cpp
    TriangleMesh.cpp)
if(JPEG_FOUND)
    set(header
        ${header}
        JPEG.h
        JPEGInline.h)
    set(source
        ${source}
        JPEG.cpp
        JPEGRead.cpp
        JPEGWrite.cpp)
endif()
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
if(TIFF_FOUND)
    set(header
        ${header}
        TIFF.h
        TIFFInline.h)
    set(source
        ${source}
        TIFF.cpp
        TIFFRead.cpp
        TIFFWrite.cpp)
endif()

add_library(djvAV ${header} ${source})
set(LIBRARIES
    glbinding
    GLFW
    FreeType
    IlmBase
    OpenAL
    OpenGL::GL
    djvCore)
if(JPEG_FOUND)
    set(LIBRARIES ${LIBRARIES} JPEG)
endif()
if(PNG_FOUND)
    set(LIBRARIES ${LIBRARIES} PNG)
endif()
if(TIFF_FOUND)
    set(LIBRARIES ${LIBRARIES} TIFF)
endif()
target_link_libraries(djvAV ${LIBRARIES})
set_target_properties(
    djvAV
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)


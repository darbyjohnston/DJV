set(header
    Audio.h
    AudioData.h
    AudioDataInline.h
    AudioInline.h
    Context.h
    Color.h
    FFmpeg.h
    IO.h
    IOInline.h
    Pixel.h
    PixelData.h
    PixelDataInline.h
    PixelInline.h
    Image.h
    ImageTags.h)
set(mocHeader
    Audio.h
    AudioData.h
    Color.h
    Context.h
    IO.h
    Pixel.h
    PixelData.h)
set(source
    Audio.cpp
    AudioData.cpp
    Context.cpp
    Color.cpp
    FFmpeg.cpp
    IO.cpp
    Pixel.cpp
    PixelData.cpp
    Image.cpp
    ImageTags.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_library(djvViewExperimentAV ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewExperimentAV djvViewExperimentCore)
set_target_properties(
    djvViewExperimentAV
    PROPERTIES
    FOLDER experiments/djvViewExperiment
    CXX_STANDARD 11)


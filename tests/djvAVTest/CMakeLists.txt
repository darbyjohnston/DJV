set(header
    AVSystemTest.h
    CineonTest.h
    DPXTest.h
    IOTest.h
    PPMTest.h
	SpeedTest.h
    ThumbnailSystemTest.h
    TimeTest.h)
set(source
    AVSystemTest.cpp
    CineonTest.cpp
    DPXTest.cpp
    IOTest.cpp
    PPMTest.cpp
	SpeedTest.cpp
    ThumbnailSystemTest.cpp
    TimeTest.cpp)
if (NOT DJV_BUILD_TINY AND NOT DJV_BUILD_MINIMAL)
    if(FFmpeg_FOUND)
        set(header
            ${header}
            FFmpegTest.h)
        set(header
            ${header}
            FFmpegTest.cpp)
    endif()
    if(JPEG_FOUND)
        set(header
            ${header}
            JPEGTest.h)
        set(header
            ${header}
            JPEGTest.cpp)
    endif()
    if(OpenEXR_FOUND)
        set(header
            ${header}
            OpenEXRTest.h)
        set(header
            ${header}
            OpenEXRTest.cpp)
    endif()
    if(TIFF_FOUND)
        set(header
            ${header}
            TIFFTest.h)
        set(header
            ${header}
            TIFFTest.cpp)
    endif()
endif()

add_library(djvAVTest ${header} ${source})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


set(header
    AVSystemTest.h
    CineonFuncTest.h
    DPXFuncTest.h
    IOTest.h
    PPMFuncTest.h
	SpeedFuncTest.h
    ThumbnailSystemTest.h
    TimeFuncTest.h)
set(source
    AVSystemTest.cpp
    CineonFuncTest.cpp
    DPXFuncTest.cpp
    IOTest.cpp
    PPMFuncTest.cpp
	SpeedFuncTest.cpp
    ThumbnailSystemTest.cpp
    TimeFuncTest.cpp)
if (NOT DJV_BUILD_TINY AND NOT DJV_BUILD_MINIMAL)
    if(FFmpeg_FOUND)
        set(header
            ${header}
            FFmpegFuncTest.h)
        set(header
            ${header}
            FFmpegFuncTest.cpp)
    endif()
    if(JPEG_FOUND)
        set(header
            ${header}
            JPEGFuncTest.h)
        set(header
            ${header}
            JPEGFuncTest.cpp)
    endif()
    if(OpenEXR_FOUND)
        set(header
            ${header}
            OpenEXRFuncTest.h)
        set(header
            ${header}
            OpenEXRFuncTest.cpp)
    endif()
    if(TIFF_FOUND)
        set(header
            ${header}
            TIFFFuncTest.h)
        set(header
            ${header}
            TIFFFuncTest.cpp)
    endif()
endif()

add_library(djvAVTest ${header} ${source})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


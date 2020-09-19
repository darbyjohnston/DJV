set(header
    AVSystemTest.h
    CineonFuncTest.h
    DPXFuncTest.h
    IOTest.h
    PPMTest.h
	SpeedFuncTest.h
    ThumbnailSystemTest.h
    TimeFuncTest.h)
set(source
    AVSystemTest.cpp
    CineonFuncTest.cpp
    DPXFuncTest.cpp
    IOTest.cpp
    PPMTest.cpp
	SpeedFuncTest.cpp
    ThumbnailSystemTest.cpp
    TimeFuncTest.cpp)
if (NOT DJV_BUILD_TINY AND NOT DJV_BUILD_MINIMAL)
    set(header
        ${header}
        FFmpegTest.h
        JPEGTest.h
        OpenEXRTest.h
        TIFFTest.h)
    set(header
        ${header}
        FFmpegTest.cpp
        JPEGTest.cpp
        OpenEXRTest.cpp
        TIFFTest.cpp)
endif()

add_library(djvAVTest ${header} ${source})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


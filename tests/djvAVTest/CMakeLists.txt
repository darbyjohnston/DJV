set(header
    PixelTest.h)
set(mocHeader)
set(source
    PixelTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvAVTest ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvAVTest djvTestLib djvAV)
set_target_properties(
    djvAVTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


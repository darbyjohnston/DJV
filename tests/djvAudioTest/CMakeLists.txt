set(header
    AudioSystemTest.h
    DataTest.h
    InfoTest.h
    TypeTest.h)
set(source
    AudioSystemTest.cpp
    DataTest.cpp
    InfoTest.cpp
    TypeTest.cpp)

add_library(djvAudioTest ${header} ${source})
target_link_libraries(djvAudioTest djvTestLib djvAudio)
set_target_properties(
    djvAudioTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


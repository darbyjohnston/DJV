set(header
    AudioDataFuncTest.h
    AudioDataTest.h
    AudioFuncTest.h
    AudioSystemFuncTest.h
    AudioSystemTest.h
    AudioTest.h)
set(source
    AudioDataFuncTest.cpp
    AudioDataTest.cpp
    AudioFuncTest.cpp
    AudioSystemFuncTest.cpp
    AudioSystemTest.cpp
    AudioTest.cpp)

add_library(djvAudioTest ${header} ${source})
target_link_libraries(djvAudioTest djvTestLib djvAudio)
set_target_properties(
    djvAudioTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


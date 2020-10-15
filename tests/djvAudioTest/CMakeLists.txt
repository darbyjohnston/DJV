set(header
    AudioSystemFuncTest.h
    AudioSystemTest.h
    DataFuncTest.h
    DataTest.h
    InfoTest.h
    TypeFuncTest.h
    TypeTest.h)
set(source
    AudioSystemFuncTest.cpp
    AudioSystemTest.cpp
    DataFuncTest.cpp
    DataTest.cpp
    InfoTest.cpp
    TypeFuncTest.cpp
    TypeTest.cpp)

add_library(djvAudioTest ${header} ${source})
target_link_libraries(djvAudioTest djvTestLib djvAudio)
set_target_properties(
    djvAudioTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)


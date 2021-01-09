find_package(RtAudio REQUIRED)

set(source main.cpp)

add_executable(RtAudioTest ${header} ${source})
target_link_libraries(RtAudioTest RtAudio)

add_test(RtAudioTest RtAudioTest)

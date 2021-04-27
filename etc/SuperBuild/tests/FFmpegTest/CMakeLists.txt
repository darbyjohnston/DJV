find_package(FFmpeg REQUIRED)

set(source main.cpp)

add_executable(FFmpegTest ${header} ${source})
target_link_libraries(FFmpegTest FFmpeg)

add_test(FFmpegTest FFmpegTest)

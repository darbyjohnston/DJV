find_package(OpenImageDenoise REQUIRED)

set(source main.cpp)

add_executable(OpenImageDenoiseTest ${header} ${source})
target_link_libraries(OpenImageDenoiseTest OpenImageDenoise)

add_test(OpenImageDenoiseTest OpenImageDenoiseTest)

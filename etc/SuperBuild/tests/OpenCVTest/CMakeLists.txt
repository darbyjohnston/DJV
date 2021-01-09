find_package(OpenCV REQUIRED)

set(source main.cpp)

add_executable(OpenCVTest ${header} ${source})
target_link_libraries(OpenCVTest OpenCV)

add_test(OpenCVTest OpenCVTest)

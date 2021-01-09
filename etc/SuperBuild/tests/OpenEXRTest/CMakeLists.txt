find_package(OpenEXR REQUIRED)

set(source main.cpp)

add_executable(OpenEXRTest ${header} ${source})
target_link_libraries(OpenEXRTest OpenEXR)

add_test(OpenEXRTest OpenEXRTest)

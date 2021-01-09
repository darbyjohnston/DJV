find_package(openvkl REQUIRED)

set(source main.cpp)

add_executable(openvklTest ${header} ${source})
target_link_libraries(openvklTest openvkl::openvkl)

add_test(openvklTest openvklTest)
